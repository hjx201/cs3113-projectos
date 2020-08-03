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

#include "Util.h"
#include "Entity.h"

struct GameState {
    Entity* player;
    Entity* walls;

    Entity** enemies;
    Entity** bullets;

    int nextScene;
};

//GLuint fontTextureID = Util::LoadTexture("font1.png");

class Scene {
public:
    GLuint textureID = 0;

    GameState state;
    virtual void Initialize() = 0;
    virtual void ProcessInput(SDL_Event event, float cursorx, float cursory) = 0;
    virtual glm::mat4 Update(float deltaTime, glm::mat4 viewMatrix) = 0;
    virtual void Render(ShaderProgram* program) = 0;
    virtual void RenderUI(ShaderProgram* program) = 0;
};