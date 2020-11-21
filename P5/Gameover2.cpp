#include "Gameover2.h"

#define Gameover2_WIDTH 14
#define Gameover2_HEIGHT 8

unsigned int Gameover2_data[] =
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

void Gameover2::Initialize() {

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 0, 0);
    state.player->isActive = false;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(Gameover2_WIDTH, Gameover2_HEIGHT, Gameover2_data, mapTextureID, 1.0f, 4, 1);

}

void Gameover2::Update(float deltaTime) {

}
void Gameover2::Render(ShaderProgram* program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "YOU FAILED TO ESCAPE", .8, -.4, glm::vec3(-4, 0, 0));
}