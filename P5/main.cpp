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

#include "Entity.h"
#include "Util.h"
#include "Scene.h"

#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Winscreen.h"
#include "Losescreen.h"

#define NUM_SCENES 6

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[NUM_SCENES];

Mix_Music* music;
Mix_Chunk* oof;
Mix_Chunk* tada;

int lives = 3;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
    viewMatrix = glm::mat4(1.0f);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("McDoodles!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("newfriendly.wav");
    oof = Mix_LoadWAV("oof.wav");
    tada = Mix_LoadWAV("tada.wav");

    Mix_PlayMusic(music, -1);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 15);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new Winscreen();
    sceneList[5] = new Losescreen();
    SwitchToScene(sceneList[0]);
}

void ProcessInput() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        default:
            currentScene->ProcessInput(event); //processinput is handled individually
            break;
        }
    }
    
}

float lastTicks = 0.0f;

#define FIXED_TIMESTEP 0.0166666f
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        viewMatrix = currentScene->Update(FIXED_TIMESTEP, viewMatrix);

        /*
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.player);
        }
        */

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;


    if (currentScene->state.player->isDead) {
         lives -= 1;
         Mix_PlayChannel(1, oof, 0);
         Mix_VolumeChunk(oof, MIX_MAX_VOLUME / 15);
         currentScene->Initialize();
    }

    if (currentScene->isWon) {
        Mix_PlayChannel(1, tada, 0);
        Mix_VolumeChunk(tada, MIX_MAX_VOLUME / 15);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);
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

        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);

        if (lives == 0) { 
            SwitchToScene(sceneList[5]);
            lives = 3;
        };

        if (currentScene == sceneList[0]) {
            lives = 3;
        }

        Render();
    }

    Shutdown();
    return 0;
}
