#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    rotation = 0;

    modelMatrix = glm::mat4(1.0f);
}

void Entity::AI(float deltaTime, Entity* player, int gameheight, int gamewidth, glm::vec3 prevpos) {
     position.y += velocity.y * deltaTime;        // Move on Y
     position.x += velocity.x * deltaTime;        // Move on Y
     rotation += spin;
     CheckColWall(gameheight, gamewidth, prevpos);//checkColWall;
}

void Entity::Update(float deltaTime, Entity* player, Entity** objects, int objectCount, int gameheight, int gamewidth, float mousex, float mousey)
{
    if (playDead) {
        playDead = false;
    }

    if (!isActive) return;

    glm::vec3 prevpos = position;

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

    switch (entityType) {
    case ENEMY:
        AI(deltaTime, player, gameheight, gamewidth, prevpos);
        break;
    case BULLET:
        //position.x = position.x + cos(glm::radians(rotation)) * speed * deltaTime;
        //position.y = position.y + sin(glm::radians(rotation)) * speed * deltaTime;
        //move the bullet
        position.y += velocity.y * speed * deltaTime;
        position.x += velocity.x * speed * deltaTime;

        //if bullet collides with wall, delete it
        CheckColBullets(objects, objectCount);
        CheckColWall(gameheight, gamewidth, prevpos);
        break;
    case PLAYER:
        rotation = glm::degrees(atan2f(mousey - position.y, mousex - position.x));
        velocity += acceleration * deltaTime;
        position.y += velocity.y * deltaTime;        // Move on Y
        position.x += velocity.x * deltaTime;        // Move on X
        CheckColWall(gameheight, gamewidth, prevpos);
        if(!isInvincible) CheckColEnemies(objects, objectCount);
        break;
    default:
        break;
    }

    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    if (entityType == WALL) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1.0f));
    }

    if (isDead) {
        playDead = true;
    }

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

                //collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;

                //collidedBottom = true;
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

                //collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;

                //collidedLeft = true;
            }
        }
    }
}

void Entity::CheckColWall(int gameheight, int gamewidth, glm::vec3 prevpos)  {
    switch (entityType) {
    case BULLET:
        if (fabs(position.x) + width / 2 > gamewidth / 2 || fabs(position.y) + height / 2 > gameheight / 2) {
            isActive = false;
        }
        break;
    case ENEMY:
    case PLAYER:
        if (inFrame) {
            if (fabs(position.x) + width / 2 > gamewidth / 2) {
                position.x = prevpos.x;
                velocity.x *= -1;
            }
            if (fabs(position.y) + height / 2 > gameheight / 2) {
                position.y = prevpos.y;
                velocity.y *= -1;
            }
        }
        else {
            if (fabs(position.x) + width / 2 < gamewidth / 2 && fabs(position.y) + height / 2 < gameheight / 2) {
                inFrame = true;
            }
        }
    break;
    }
}

/*

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }

    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }


}
*/

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

void Entity::CheckColEnemies(Entity** enemies, int enemycount) { 

    for (int i = 0; i < enemycount; i++) {
        
        Entity* curren = enemies[i];

        if (CheckCollision(curren) && curren->isActive) {
            float ydist = fabs(position.y - curren->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (curren->height / 2.0f));
            curren->isActive = false;
            isDead = true;
        }

    }
}

void Entity::CheckColBullets(Entity** enemies, int enemycount){
    for (int i = 0; i < enemycount; i++) {

        Entity* curren = enemies[i];

        if (CheckCollision(curren) && curren->isActive) {
            float ydist = fabs(position.y - curren->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (curren->height / 2.0f));

            curren->isActive = false;
            isActive = false;
        }
    }
}

//check collision method for bullets