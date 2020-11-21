#include "Menu.h"

#define Menu_WIDTH 14
#define Menu_HEIGHT 8

unsigned int Menu_data[] =
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

void Menu::Initialize() {

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->movement = glm::vec3(0);
    state.player->life = 3;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(Menu_WIDTH, Menu_HEIGHT, Menu_data, mapTextureID, 1.0f, 4, 1);

    state.enter = false;
}

void Menu::Update(float deltaTime) {
    if (state.enter) {
        state.nextScene = 1;
    }
}
void Menu::Render(ShaderProgram* program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Escape", 1, -0.5, glm::vec3(3.5, -2, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Press Enter to Start", 0.8f, -0.5, glm::vec3(2, -4, 0));
}