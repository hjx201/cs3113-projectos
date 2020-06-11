#include <vector>

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 22

struct GameState {

    Entity* player;
    Entity* platforms;
    Entity* victorythings;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;

GLuint fontTextureID;

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

bool gameOver;

GLuint* balloonskins = new GLuint[8];

void randomizeBalloon() {
    int random = rand() % 4;

    switch (random) {
    case 0:
        state.player->alivetexture = balloonskins[0];
        state.player->deadtexture = balloonskins[1];
        break;
    case 1:
        state.player->alivetexture = balloonskins[2];
        state.player->deadtexture = balloonskins[3];
        break;
    case 2:
        state.player->alivetexture = balloonskins[4];
        state.player->deadtexture = balloonskins[5];
        break;
    case 3:
        state.player->alivetexture = balloonskins[6];
        state.player->deadtexture = balloonskins[7];
        break;
    }

    //state.player->alivetexture = balloonskins[2 * random];
    //state.player->deadtexture = balloonskins[random + 1];
}


void resetPos() {
    state.player->position = glm::vec3(0, 2.75f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.2f, 0);
    state.player->velocity = glm::vec3(0);
    randomizeBalloon();

    state.player->isDead = false;
    state.player->isWon = false;

    gameOver = false;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Birthday Party!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects
    balloonskins[0] = LoadTexture("balloon.png");
    balloonskins[1] = LoadTexture("balloon_dead.png");
    balloonskins[2] = LoadTexture("balloon2.png");
    balloonskins[3] = LoadTexture("balloon2_dead.png");
    balloonskins[4] = LoadTexture("balloon3.png");
    balloonskins[5] = LoadTexture("balloon3_dead.png");
    balloonskins[6] = LoadTexture("balloon4.png");
    balloonskins[7] = LoadTexture("balloon4_dead.png");


    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 2.75f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.2f, 0);
    state.player->velocity = glm::vec3(0);
    state.player->speed = 1.5f;
    
    randomizeBalloon();
    

    state.player->width = .5;
    state.player->height = .5;

    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("spikes_bot.png");
    
    for (int i = 0; i < 6; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(i - 4.5, -3.25f, 0);
    }

    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].position = glm::vec3(3.5, -3.25f, 0);

    state.platforms[7].textureID = platformTextureID;
    state.platforms[7].position = glm::vec3(4.5, -3.25f, 0);

    state.platforms[20].textureID = platformTextureID;
    state.platforms[20].position = glm::vec3(-2.5, -1.25f, 0);
    
    state.platforms[21].textureID = platformTextureID;
    state.platforms[21].position = glm::vec3(0.5, 0.25f, 0);




    platformTextureID = LoadTexture("spikes_left.png");

    for (int i = 0; i < 6; i++) {
        state.platforms[8 + i].textureID = platformTextureID;
        state.platforms[8 + i].position = glm::vec3(-4.5, i - 2, 0);
    }

    platformTextureID = LoadTexture("spikes_right.png");
    for (int i = 0; i < 6; i++) {
        state.platforms[14 + i].textureID = platformTextureID;
        state.platforms[14 + i].position = glm::vec3(4.5, i - 2, 0);
    }

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0, NULL);
        state.platforms[i].entype = entypes::SPIKES;
    }

    state.victorythings = new Entity[2];

    GLuint vicTextureID = LoadTexture("birthdayzone1.png");
    state.victorythings[0].textureID = vicTextureID;
    state.victorythings[0].position = glm::vec3(1.5, -3.25f, 0);

    vicTextureID = LoadTexture("birthdayzone2.png");
    state.victorythings[1].textureID = vicTextureID;
    state.victorythings[1].position = glm::vec3(2.5, -3.25f, 0);

    for (int i = 0; i < 2; i++) {
        state.victorythings[i].Update(0, NULL, 0, NULL);
        state.victorythings[i].entype = entypes::VICTORYTHING;
    }

    fontTextureID = LoadTexture("font1.png");
}

void ProcessInput() {

    state.player->movement = glm::vec3(0);

    state.player->acceleration.x = 0.0f;

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
                if (gameOver) {
                    resetPos();
                }

                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (!state.player->collidedBottom || !state.player->collidedLeft || !state.player->collidedRight) {
            state.player->acceleration.x = -1.0f;
        }
        //state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!state.player->collidedBottom || !state.player->collidedLeft || !state.player->collidedRight) {
            state.player->acceleration.x = 1.0f;
        }
        //state.player->animIndices = state.player->animRight;
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.victorythings);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    if (state.player->isDead || state.player->isWon) {
        gameOver = true;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    state.victorythings[0].Render(&program);
    state.victorythings[1].Render(&program);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    state.player->Render(&program);

    if (state.player->isDead) {
        DrawText(&program, fontTextureID, "mission failed :(", 0.5f, -0.25f,
            glm::vec3(-2.75f, 0, 0));
    }
    else if (state.player->isWon) {
        DrawText(&program, fontTextureID, "mission successful! :)", 0.5f, -0.25f,
            glm::vec3(-2.75f, 0, 0));
    }

    if (gameOver) {
        DrawText(&program, fontTextureID, "[press space to reset]", 0.5f, -0.25f,
            glm::vec3(-2.75f, -1, 0));
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

