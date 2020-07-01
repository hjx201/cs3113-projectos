#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;

    timeActive = 0;
    modelMatrix = glm::mat4(1.0f);
}

void Entity::AIRoach(Entity* platforms, int platformCount, float deltaTime) {

    //acceleration = glm::vec3(0, -2.81, 0);

    velocity.x = movement.x * speed;
    //velocity.y += acceleration.y * deltaTime;
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed


    if (dir == RIGHT) {
        isMirrored = -1;
        movement = glm::vec3(1, 0, 0);
    }
    else {
        isMirrored = 1;
        movement = glm::vec3(-1, 0, 0);
    }

    if (collidedLeft) {
        dir = RIGHT;
    }
    else if (collidedRight) {
        dir = LEFT;
    }
}

void Entity::AIButterfly(float deltaTime) {
    if (position.x - width/2 < -5) {
        dir = RIGHT;
    }
    else if (position.x + width / 2 > 5) {
        dir = LEFT;
    }

    if (dir == RIGHT) {
        isMirrored = -1;
    }
    else if (dir == LEFT) {
        isMirrored = 1;
    }
    timeActive += deltaTime;
    
    movement = (glm::vec3(-1*isMirrored, 3*sin(timeActive * 3.14), 0));

    velocity = movement * speed;
    position += velocity * deltaTime; 
}

void Entity::AIWasp(Entity* player, float deltaTime) {
    switch (aiState) {
    case WAITING:
        timeActive += deltaTime;
        
        if (timeActive > 2.25) {
            target = player->position;
            aiState = ATTACKING;
        }
        break;
    case ATTACKING:
        movement = glm::normalize(target-position);

        if (movement.x < 0) {
            dir = LEFT;
            isMirrored = 1;
        }
        else if (movement.y < 0) {
            dir = RIGHT;
            isMirrored = -1;
        }


        velocity = movement * speed;
        position += velocity * deltaTime;

        if (glm::distance(position, target) < 0.1) {
            aiState = WAITING;
            timeActive = 0;
        }
        break;
    }

}


void Entity::AI(Entity* player, Entity* platforms, int platformCount, float deltaTime) {
    switch (aiType) {
    case BUTTERFLY:
        AIButterfly(deltaTime);
        break;
    case ROACH:
        AIRoach(platforms, platformCount, deltaTime);
        break;
    case WASP:
        AIWasp(player, deltaTime);
        break;
    }

}

void Entity::Update(float deltaTime, Entity *platforms, int platformCount, Entity* player, Entity* enemies, int enemycount)
{

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;


    if (!isActive) return;

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    if (entityType == PLAYER) {

        if (jump) {
            jump = false;

            velocity.y += jumpPower;
        }


        velocity.x = movement.x * speed;

        velocity += acceleration * deltaTime;

        position.y += velocity.y * deltaTime; // Move on Y
        CheckCollisionsY(platforms, platformCount);// Fix if needed

        position.x += velocity.x * deltaTime; // Move on X
        CheckCollisionsX(platforms, platformCount);// Fix if needed

        CheckColEnemies(enemies, enemycount);
    }
    if (entityType == ENEMY) {
        AI(player, platforms, platformCount, deltaTime);
    }

    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(isMirrored, 1, 1));
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;

                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;

                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;

                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;

                collidedLeft = true;
            }
        }
    }
}


void Entity::Render(ShaderProgram* program) {
    if (!isActive) return;

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


bool Entity::CheckCollision(Entity* other) {
    if (!isActive || !other->isActive) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) return true;

    return false;
}


void Entity::CheckColEnemies(Entity* enemies, int enemycount) {
    
    for (int i = 0; i < enemycount; i++) {
        collidedBottomE = false;

        Entity* curren = &enemies[i];

        if (CheckCollision(curren) && curren->isActive) {
            float ydist = fabs(position.y - curren->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (curren->height / 2.0f));
            if (velocity.y < 0 && position.y > curren->position.y) {
                collidedBottomE = true;
            }



            if (collidedBottomE) {
                curren->isActive = false;
            }
            else {
                isDead = true;
            }
         }

    }
}