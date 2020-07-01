#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>

#include "Entity.h"

#define PLATFORM_COUNT 12
#define ENEMY_COUNT 3


struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

GameState state;

bool isPaused = true;
enum PauseState {WIN, LOSE, START, UNPAUSED};

PauseState pstate = START;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return textureID;
}

GLuint fontTextureID;

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;

        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
          offset + (-0.5f * size), 0.5f * size,
          offset + (-0.5f * size), -0.5f * size,
          offset + (0.5f * size), 0.5f * size,
          offset + (0.5f * size), -0.5f * size,
          offset + (0.5f * size), 0.5f * size,
          offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });

    } // end of for loop

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void ResetBoard() {
    state.player->position = glm::vec3(-4, -1, 0);
    state.player->isDead = false;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].isActive = true;
    }

    state.enemies[0].position = glm::vec3(3.25, -2.39, 0);
    state.enemies[0].dir = LEFT;

    state.enemies[1].position = glm::vec3(0, -1.25, 0);
    state.enemies[1].dir = RIGHT;
    state.enemies[1].timeActive = 0;

    state.enemies[2].position = glm::vec3(4, -2.25, 0);
    state.enemies[2].aiState = WAITING;
    state.enemies[2].timeActive = 0;

}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Jumping Spider's Mealtime!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -1, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->velocity = glm::vec3(0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("jumpingspider.png");

    /*
    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    */

    state.player->width = 0.8f;
    state.player->height = 0.69f;

    state.player->jumpPower = 6;

    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

    for (int i = 0; i < PLATFORM_COUNT - 2; i++) {
        state.platforms[i].position = glm::vec3(-4.5 + i, -3.25f, 0);
    }

    state.platforms[10].position = glm::vec3(-2.5, -2.25f, 0);
    state.platforms[11].position = glm::vec3(4.5, -2.25f, 0);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].Update(0, NULL, 0, NULL, NULL, 0);
    }

    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("roach.png");

    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(3.25, -2.39, 0);

    state.enemies[0].aiType = ROACH;
    state.enemies[0].dir = LEFT;
    state.enemies[0].speed = 1;

    state.enemies[0].acceleration = glm::vec3(0, -9.81, 0);

    state.enemies[0].width = 0.8;
    state.enemies[0].height = 0.5;

    
    enemyTextureID = LoadTexture("butterfly.png");

    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].position = glm::vec3(0, -1.25, 0);

    state.enemies[1].aiType = BUTTERFLY;
    state.enemies[1].speed = 1.5;
    state.enemies[1].dir = RIGHT;
    state.enemies[1].width = 0.6;
    state.enemies[1].height = 0.6;
    
    enemyTextureID = LoadTexture("wasp.png");

    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].position = glm::vec3(4, -2.25, 0);

    state.enemies[2].aiType = WASP;
    state.enemies[2].aiState = WAITING;
    state.enemies[2].speed = 4;
    state.enemies[2].width = 0.7;
    state.enemies[2].height = 0.7;
    

    fontTextureID = LoadTexture("font1.png");
}

void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                // Some sort of action
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                }
                break;
            case SDLK_s:
                switch (pstate) {
                case START:
                    pstate = UNPAUSED;
                    isPaused = false;
                    ResetBoard();
                    break;
                case WIN:
                    pstate = START;
                    break;
                case LOSE: 
                    pstate = START;
                    break;
                case UNPAUSED:
                    break;
                }
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

float lastTicks = 0.0f;

#define FIXED_TIMESTEP 0.0166666f
float accumulator = 0.0f;

void Update() {
    if (!isPaused) {

        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;

        int numlivingenemies = 0;

        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }

        while (deltaTime >= FIXED_TIMESTEP) {
            // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.player, state.enemies, ENEMY_COUNT);

            for (int i = 0; i < ENEMY_COUNT; i++) {

                state.enemies[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.player, NULL, 0);
            }

            deltaTime -= FIXED_TIMESTEP;

            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (state.enemies[i].isActive == true) numlivingenemies++;
            }

            if (numlivingenemies == 0) {
                isPaused = true;
                pstate = WIN;
            }
            
            if (state.player->isDead) {
                isPaused = true;
                pstate = LOSE;
            }
        }

        accumulator = deltaTime;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

    switch (pstate) {
    case WIN:
        DrawText(&program, fontTextureID, "you win! spider is happy :)", 0.5f, -0.25f,
            glm::vec3(-2.75f, 0, 0));
        DrawText(&program, fontTextureID, "[press s to reset]", 0.5f, -0.25f,
            glm::vec3(-2.75f, -1, 0));
        break;
    case LOSE: 
        DrawText(&program, fontTextureID, "you lose! spider is dead :(", 0.5f, -0.25f,
            glm::vec3(-2.75f, 0, 0));
        DrawText(&program, fontTextureID, "[press s to reset]", 0.5f, -0.25f,
            glm::vec3(-2.75f, -1, 0));
        break;
    case START:
        DrawText(&program, fontTextureID, "help spider pounce on its food!", 0.5f, -0.25f,
            glm::vec3(-3.25f, 0, 0));
        DrawText(&program, fontTextureID, "[press s to start]", 0.5f, -0.25f,
            glm::vec3(-2.75f, -1, 0));
        break;
    case UNPAUSED:
        break;
    }


    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
