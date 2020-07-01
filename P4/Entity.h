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


#include <math.h>

enum EntityType {PLAYER, PLATFORM, ENEMY};

enum AIType {ROACH, WASP, BUTTERFLY};

enum AIState {ATTACKING, WAITING};

enum Direction {LEFT, RIGHT};

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    Direction dir;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;

    float width = 1;
    float height = 1;

    bool jump = false;
    float jumpPower = 0;

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

    bool isActive = true;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    int isMirrored = 1;

    float timeActive;

    glm::vec3 target;

    bool collidedBottomE = false;
    bool collidedRightE = false;
    bool collidedLeftE = false;
    bool collidedTopE = false;

    bool isDead = false;

    Entity();

    bool CheckCollision(Entity* other);

    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);


    void Update(float deltaTime, Entity* platforms, int platformCount, Entity* player, Entity* enemies, int enemycount);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    void AI(Entity* player, Entity* platforms, int platformCount, float deltaTime);

    void AIRoach(Entity* platforms, int platformCount, float deltaTime);
    void AIButterfly(float deltaTime);
    void AIWasp(Entity* player, float deltaTime);

    void CheckColEnemies(Entity* enemies, int enemycount);
};
