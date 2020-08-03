#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType {PLAYER, ENEMY, BULLET, WALL};

//enum AIType {PRIUS};

//enum AIState {IDLE, MOVING};

class Entity {
public:
    EntityType entityType;
    //AIType aiType;
    //AIState aiState;
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed = 0;
    float rotation = 0;

    float spin = 0;

    float width = 1;
    float height = 1;

    GLuint textureID;

    glm::mat4 modelMatrix;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    bool isActive = false;

    bool isDead = false;
    bool playDead = false;

    bool detectWalls = false;

    bool inFrame = false;

    bool isInvincible = false;
    //bool collidedTop = false;
    //bool collidedBottom = false;
    //bool collidedLeft = false;
    //bool collidedRight = false;

    Entity();

    bool CheckCollision(Entity* other);

    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    void CheckColWall(int gameheight, int gamewidth, glm::vec3 prevpos);

    //void CheckCollisionsX(Map* map);
    //void CheckCollisionsY(Map* map);  //change to wall?

    void Update(float deltaTime, Entity* player, Entity** objects, int objectCount, int gameheight, int gamewidth, float, float);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    void AI(float, Entity* player, int, int, glm::vec3);
    //void AIPrius(Entity* player);

    void CheckColEnemies(Entity** enemies, int enemycount);
    //check collisions btwn enemies and bullets

    void CheckColBullets(Entity** enemies, int enemycount);
};
