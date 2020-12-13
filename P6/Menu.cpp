#include "Menu.h"

void Menu::Initialize() {

    state.nextScene = -1;

    state.uiViewMatrix = glm::mat4(1.0);
    state.uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);

    state.fontTextureID = Util::LoadTexture("font1.png");

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->isActive = true;

    state.enter = false;
}

void Menu::Update(float deltaTime) {
    if (state.enter) {
        state.nextScene = 1;
    }
    state.player->Update(deltaTime, state.player, nullptr, 0, nullptr, 0);
}
void Menu::Render(ShaderProgram* program) {
    program->SetProjectionMatrix(state.uiProjectionMatrix);
    program->SetViewMatrix(state.uiViewMatrix);

    Util::DrawText(program, state.fontTextureID, "ONLY SURVIVE", 1, -0.3f, glm::vec3(-3.5, 2, 0));
    Util::DrawText(program, state.fontTextureID, "You have 3 lives, you're mission is to survive all 12 rounds", 0.5f, -0.3f, glm::vec3(-6, 0, 0));
    Util::DrawText(program, state.fontTextureID, " press space to shoot, AWSD to move", 0.5f, -0.3f, glm::vec3(-3, -0.5f, 0));
    Util::DrawText(program, state.fontTextureID, "Press Enter to Start", 0.75f, -0.4f, glm::vec3(-3, -2, 0));
}