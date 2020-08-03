#pragma once
#include "Scene.h"

class Game : public Scene {
    struct Wave {
        float min_asteroid_speed;
        float max_asteroid_speed;
        int cooldown;
        int enemiesRemaining;
        int maxencount;
    };

public:
    Wave* waves;
    int currentWave;
    int enemiesRemaining;

    //glm::vec3* ViewAnchors;

    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    float viewx = 0.0f; //current location of viewmatrix
    float viewy = 0.0f;

    bool isFiring;
    //have int of current active enemies
    int activeBullets;//have int of current active bullets
    int activeAsteroids;

    float bulletCooldown;
    float asterCooldown;
    float invulnerabletime;
    //cooldown ints (can be private?)

    void Initialize() override;
    void ProcessInput(SDL_Event event, float, float) override;
    glm::mat4 Update(float deltaTime, glm::mat4 viewMatrix) override;
    void Render(ShaderProgram* program) override;
    void RenderUI(ShaderProgram* program) override;

    glm::mat4 UpdateViewMatrix(glm::mat4 viewMatrix);

    void initWalls();
    void initPlayer();
    void initAsteroids();
    void initBullets();

    void initWaves();

    void fire(float, float);
    void spawnAsteroid();

    int lives;

    int rearrange(Entity** objects, int);

    GLuint fontTextureID = Util::LoadTexture("font1.png");


};