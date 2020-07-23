#pragma once

#include "Scene.h"

class Level3 : public Scene {

public:
    void Initialize() override;
    void ProcessInput(SDL_Event event) override;
    glm::mat4 Update(float deltaTime, glm::mat4 viewMatrix) override;
    void Render(ShaderProgram* program) override;
};