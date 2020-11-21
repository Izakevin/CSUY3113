#include "Win.h"

#define Win_WIDTH 14
#define Win_HEIGHT 8

unsigned int Win_data[] =
{
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void Win::Initialize() {

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 0, 0);
    state.player->isActive = false;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(Win_WIDTH, Win_HEIGHT, Win_data, mapTextureID, 1.0f, 4, 1);

}

void Win::Update(float deltaTime) {

}
void Win::Render(ShaderProgram* program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "YOU ESCAPED", 1, -.5, glm::vec3(-2, 0, 0));
}