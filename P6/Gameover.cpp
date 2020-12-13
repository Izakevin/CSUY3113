#include "Gameover.h"

void Gameover::Initialize() {

    state.nextScene = -1;

    state.uiViewMatrix = glm::mat4(1.0);
    state.uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);

    state.fontTextureID = Util::LoadTexture("font1.png");

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->isActive = false;
}

void Gameover::Update(float deltaTime) {

}

void Gameover::Render(ShaderProgram* program) {
    program->SetProjectionMatrix(state.uiProjectionMatrix);
    program->SetViewMatrix(state.uiViewMatrix);
    Util::DrawText(program, state.fontTextureID, "Mission failed", 1, -0.3f, glm::vec3(-4, 0, 0));
}