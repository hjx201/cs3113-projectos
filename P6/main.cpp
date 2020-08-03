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
#include "Game.h"
#include "Winscreen.h"
#include "Losescreen.h"

#include <time.h>

#define NUM_SCENES 4

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640

#define ORTHO_HEIGHT 7.5f
#define ORTHO_WIDTH 10.0f


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, uiViewMatrix, uiProjectionMatrix;

Scene* currentScene;
Scene* sceneList[NUM_SCENES];

Mix_Music* music;
Mix_Music* losemusic;
Mix_Chunk* yay;
Mix_Chunk* quack;


void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();

    if (currentScene == sceneList[3]) {
        Mix_HaltMusic();
        Mix_PlayMusic(losemusic, -1);
    }
    else if (currentScene == sceneList[2]) {
        Mix_HaltMusic();
        Mix_PlayChannel(-1, yay, 0);
    }
    viewMatrix = glm::mat4(1.0f);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Burger Hunter!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("clubdiver.wav");
    losemusic = Mix_LoadMUS("heartbreak.mp3");
    yay = Mix_LoadWAV("yay.wav");
    quack = Mix_LoadWAV("quack.wav");

    Mix_PlayMusic(music, -1);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 15);
    Mix_VolumeChunk(yay, MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(quack, MIX_MAX_VOLUME / 4);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-ORTHO_WIDTH/2, ORTHO_WIDTH/2, -ORTHO_HEIGHT/2, ORTHO_HEIGHT/2, -1.0f, 1.0f);

    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-ORTHO_WIDTH / 2, ORTHO_WIDTH / 2, -ORTHO_HEIGHT / 2, ORTHO_HEIGHT / 2, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu(); //insert scene creations here
    sceneList[1] = new Game(); //insert scene creations here
    sceneList[2] = new Winscreen(); //insert scene creations here
    sceneList[3] = new Losescreen(); //insert scene creations here
    SwitchToScene(sceneList[0]);
}

void ProcessInput() {
    SDL_Event event;

    int mousex;
    int mousey;
    float unit_x;
    float unit_y;

    SDL_GetMouseState(&mousex, &mousey);
    //WASD? or use mouse input to determine rotation/ angle of fire
    unit_x = (((float)mousex / (float)WINDOW_WIDTH) * (float)ORTHO_WIDTH) - ((float)ORTHO_WIDTH / 2.0f);
    unit_y = ((((float) WINDOW_HEIGHT - (float)mousey) / (float)WINDOW_HEIGHT) * (float)ORTHO_HEIGHT) - ((float)ORTHO_HEIGHT / 2.0f);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        default:
            currentScene->ProcessInput(event, unit_x, unit_y); //processinput is handled individually
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
        if (currentScene->state.player->playDead) {
            Mix_PlayChannel(-1, quack, 0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);

    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    currentScene->RenderUI(&program);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(music);
    Mix_FreeMusic(losemusic);
    Mix_FreeChunk(yay);
    Mix_FreeChunk(quack);

    SDL_Quit();
}

int main(int argc, char* argv[]) {
    srand(time(NULL));//seed the rng
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();

        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);

        Render();
    }

    Shutdown();
    return 0;
}
