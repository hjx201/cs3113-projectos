#include "Losescreen.h"


void Losescreen::Initialize() {
    state.nextScene = -1;
    switchScene = false;
    fontTextureID = Util::LoadTexture("font1.png");

    isWon = false;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->isDead = false;
    state.player->isActive = false;
}

void Losescreen::ProcessInput(SDL_Event event) {
    switch (event.type) {

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_RETURN:
            switchScene = true;
            break;
        }
        break; // SDL_KEYDOWN
    }

}

glm::mat4 Losescreen::Update(float deltaTime, glm::mat4 viewMatrix) {
    if (switchScene) {
        state.nextScene = 0;
    }

    return viewMatrix;
}

void Losescreen::Render(ShaderProgram* program) {
    Util::DrawText(program, fontTextureID, "You lose!", 0.5f, -0.25f,
        glm::vec3(-2.75f, 0, 0));
    Util::DrawText(program, fontTextureID, "[press enter to restart]", 0.5f, -0.25f,
        glm::vec3(-2.75f, -1, 0));
}