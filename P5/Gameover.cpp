#include "Gameover.h"

#define Gameover_WIDTH 14
#define Gameover_HEIGHT 8

unsigned int Gameover_data[] =
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

void Gameover::Initialize() {

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 0, 0);
    state.player->isActive = false;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(Gameover_WIDTH, Gameover_HEIGHT, Gameover_data, mapTextureID, 1.0f, 4, 1);

}

void Gameover::Update(float deltaTime) {

}
void Gameover::Render(ShaderProgram* program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "YOU FAILED TO ESCAPE", .8, -.4, glm::vec3(1, -3.5, 0));
}