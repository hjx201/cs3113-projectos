#pragma once
#include "Scene.h"

class Losescreen : public Scene {

public:
    void Initialize() override;
    void ProcessInput(SDL_Event event, float cursorx, float cursory) override;
    glm::mat4 Update(float deltaTime, glm::mat4 viewMatrix) override;
    void Render(ShaderProgram* program) override;
    void RenderUI(ShaderProgram* program) override;

    bool switchScene;
    GLuint fontTextureID = Util::LoadTexture("font1.png");
};