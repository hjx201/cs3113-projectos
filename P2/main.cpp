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
#include "stb_image.h";

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;

glm::mat4 ballmat, p1mat, p2mat;

glm::mat4 bgmat;

// Start at 0, 0, 0
glm::vec3 p1_position = glm::vec3(0, 0, 0);
glm::vec3 p2_position = glm::vec3(0, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);

// Don’t go anywhere (yet).
glm::vec3 p1_movement = glm::vec3(0, 0, 0);
glm::vec3 p2_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);

//paddle/ball hitboxes
float playerh = 2.0f;
float w = 0.5f;

float player_speed = 2.0f;
float ball_speed = 2.0f;

int p1_score, p2_score = 0;

GLuint playertextid, balltextid, bgtextid;

bool initialized = false;
bool start, started = false;


Mix_Music* music;
Mix_Chunk* bounce;


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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void resetBoard() {
    p1_position.y = p2_position.y = 0;
    ball_movement = glm::vec3(0);
    ball_position = glm::vec3(0);

    ball_speed = 2.0f;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Pang!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    // Start Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("dooblydoo.mp3");
    bounce = Mix_LoadWAV("bounce.wav");
    
    Mix_PlayMusic(music, -1);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    viewMatrix = glm::mat4(1.0f);

    p1mat = glm::mat4(1.0f);
    p2mat = glm::mat4(1.0f);
    ballmat = glm::mat4(1.0f);
    bgmat = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_BLEND);

    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    playertextid = LoadTexture("spaddle.png");
    balltextid = LoadTexture("sball.png");
    bgtextid = LoadTexture("sbackground.png");

    p1_position.x = -4.5;
    p2_position.x = 4.5;

    resetBoard();

    initialized = true;


}



void ProcessInput() {
    SDL_Event event;

    p1_movement = glm::vec3(0);
    p2_movement = glm::vec3(0);

    while (SDL_PollEvent(&event)) {

        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            case SDLK_SPACE:
                // Some sort of action

                if (initialized || !started) {
                    start = true;
                    initialized = false;
                }
                
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) {
        p1_movement.y = 1.0f;
    }

    if (keys[SDL_SCANCODE_S]) {
        p1_movement.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        p2_movement.y = 1.0f;
    }

    if (keys[SDL_SCANCODE_DOWN]) {
        p2_movement.y = -1.0f;
    }

}

void startBall() {
    int startdir = rand() % 4;

    switch (startdir) {
    case 0:
        ball_movement = normalize(glm::vec3(1, 1, 0));
        break;
    case 1:
        ball_movement = normalize(glm::vec3(-1, 1, 0));
        break;

    case 2:
        ball_movement = normalize(glm::vec3(1, -1, 0));
        break;
    
    case 3:
        ball_movement = normalize(glm::vec3(-1, -1, 0));
        break;
    }

    started = true;
    start = false;
}

void checkCollisions() {
    if (ball_position.x - w / 2 <= -5) {
        p2_score += 1;
        started = false;
        resetBoard();
    }
    else if (ball_position.x + w / 2 >= 5) {
        p1_score += 1;
        started = false;
        resetBoard();
    }



    //check hitboxes
    float xdist1 = fabs((float)ball_position.x - (float)p1_position.x) - (w);
    float ydist1 = fabs((float)ball_position.y - (float)p1_position.y) - ((playerh + w) / 2.0f);


    float xdist2 = fabs((float)ball_position.x - (float)p2_position.x) - (w);
    float ydist2 = fabs((float)ball_position.y - (float)p2_position.y) - ((playerh + w) / 2.0f);


    if (xdist1 < 0 && ydist1 < 0) {
        Mix_PlayChannel(-1, bounce, 0);

        ball_movement.x = 1;
    }
    else if (xdist2 < 0 && ydist2 < 0) {
        Mix_PlayChannel(-1, bounce, 0);
        ball_movement.x = -1;
    }

}

void moveObjects(float deltaTime) {
    // Add (direction * units per second * elapsed time)

    if (p1_position.y + playerh / 2.0f >= 3.75f) {
        p1_position.y = 3.70f - playerh/2.0f;
    }
    else if (p1_position.y - playerh / 2.0f <= -3.75f) {
        p1_position.y = -3.70f + playerh/2.0f;
    }
    p1_position += p1_movement * player_speed * deltaTime;

    if (p2_position.y + playerh / 2.0f >= 3.75f) {
        p2_position.y = 3.70f - playerh / 2.0f;
    }
    else if (p2_position.y - playerh / 2.0f <= -3.75f) {
        p2_position.y = -3.70f + playerh / 2.0f;
    }
    p2_position += p2_movement * player_speed * deltaTime;


    if (ball_position.y + w / 2.0f >= 3.75f) {
        Mix_PlayChannel(-1, bounce, 0);
        ball_movement.y = -1;
    }
    if (ball_position.y - w / 2.0f <= -3.75f) {
        Mix_PlayChannel(-1, bounce, 0);
        ball_movement.y = 1;
    }
    
    ball_position += ball_movement * ball_speed * deltaTime;
}


float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    if (start) {
        startBall();
    }

    checkCollisions();

    if (started) {
        moveObjects(deltaTime);
        ball_speed += 0.005*deltaTime;
    }

    p1mat = glm::mat4(1.0f);
    p2mat = glm::mat4(1.0f);
    ballmat = glm::mat4(1.0f);
    p1mat = glm::translate(p1mat, p1_position);
    p2mat = glm::translate(p2mat, p2_position);
    ballmat = glm::translate(ballmat, ball_position);

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);


    float vertices[] = { -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(bgmat);
    glBindTexture(GL_TEXTURE_2D, bgtextid);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(p1mat);
    glBindTexture(GL_TEXTURE_2D, playertextid);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(p2mat);
    glBindTexture(GL_TEXTURE_2D, playertextid);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(ballmat);
    glBindTexture(GL_TEXTURE_2D, balltextid);
    glDrawArrays(GL_TRIANGLES, 0, 6);



    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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
