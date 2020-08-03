#include "Winscreen.h"


void Winscreen::Initialize() {
    state.nextScene = -1;
    switchScene = false;

    textureID = Util::LoadTexture("winscreen.png");

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->isDead = false;
    state.player->isActive = false;

}

void Winscreen::ProcessInput(SDL_Event event, float cursorx, float cursory) {
    /*
    switch (event.type) {

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_RETURN:
            switchScene = true;
            break;
        }
        break; // SDL_KEYDOWN
    }
    */
}

glm::mat4 Winscreen::Update(float deltaTime, glm::mat4 viewMatrix) {
    if (switchScene) {
        state.nextScene = 1;
    }

    return viewMatrix;
}

void Winscreen::Render(ShaderProgram* program) {
    //Util::DrawText(program, fontTextureID, "you win!", 0.5f, -0.25f,
      //  glm::vec3(-2.75f, 0, 0));
    //Util::DrawText(program, fontTextureID, "[press enter to start]", 0.5f, -0.25f,
     //   glm::vec3(-2.75f, -1, 0));
}

void Winscreen::RenderUI(ShaderProgram* program) {
    Util::DrawIcon(program, textureID, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 7.5f, 0.0f));
}