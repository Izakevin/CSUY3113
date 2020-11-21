#include "Level3.h"
#define LEVEL3_WIDTH 20
#define LEVEL3_HEIGHT 8

#define LEVEL3_AI_COUNT 4

Mix_Chunk* land2;

unsigned int level3_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
 3, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 0, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 3, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
 3, 2, 2, 2, 2, 0, 0, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1,
 3, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -4, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -8.81f, 0);
    state.player->velocity = glm::vec3(0);
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    state.player->isActive = true;
    state.player->life = 3;

    state.player->height = 0.8f;
    state.player->width = 0.6f;

    state.player->jpower = 5.0f;

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.enemy = new Entity[LEVEL3_AI_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].position = glm::vec3(14, 4.5, 0);
    state.enemy[0].entityType = ENEMY;
    state.enemy[0].acceleration = glm::vec3(0, -4.5f, 0);
    state.enemy[0].speed = 1;
    state.enemy[0].movement = glm::vec3(1, 0, 0);
    state.enemy[0].aiType = FOLLOWINRANGE;
    state.enemy[0].aiState = PATROL;
    state.enemy[0].height = 1.0f;
    state.enemy[0].width = 0.6f;
    state.enemy[0].turnAround = 70;

    state.enemy[1].textureID = enemyTextureID;
    state.enemy[1].position = glm::vec3(13, -3.25f, 0);
    state.enemy[1].acceleration = glm::vec3(0, -4.5f, 0);
    state.enemy[1].entityType = ENEMY;
    state.enemy[1].speed = .8;
    state.enemy[1].movement = glm::vec3(1, 0, 0);
    state.enemy[1].aiType = CHASER;
    state.enemy[1].aiState = FOLLOW;
    state.enemy[1].height = 1.0f;
    state.enemy[1].width = 0.6f;
    state.enemy[1].isActive = false;

    state.enemy[2].textureID = enemyTextureID;
    state.enemy[2].position = glm::vec3(9, 0, 0);
    state.enemy[2].acceleration = glm::vec3(0, -4.5f, 0);
    state.enemy[2].entityType = ENEMY;
    state.enemy[2].speed = 1;
    state.enemy[2].movement = glm::vec3(-1, 0, 0);
    state.enemy[2].aiType = WALKER;
    state.enemy[2].aiState = WALKING;
    state.enemy[2].height = 1.0f;
    state.enemy[2].width = 0.6f;
    state.enemy[2].turnAround = 80;

    state.enemy[3].textureID = enemyTextureID;
    state.enemy[3].position = glm::vec3(18, -4, 0);
    state.enemy[3].acceleration = glm::vec3(0, -4.5f, 0);
    state.enemy[3].entityType = ENEMY;
    state.enemy[3].speed = .8;
    state.enemy[3].movement = glm::vec3(1, 0, 0);
    state.enemy[3].aiType = FOLLOWINRANGE;
    state.enemy[3].aiState = PATROL;
    state.enemy[3].height = 1.0f;
    state.enemy[3].width = 0.6f;
    state.enemy[3].turnAround = 30;
    state.enemy[3].isActive = false;

    land2 = Mix_LoadWAV("land.wav");
    Mix_Volume(1, MIX_MAX_VOLUME / 4);
}

void Level3::Update(float deltaTime) {
    state.player->life = 0;
    state.player->Update(deltaTime, state.player, state.enemy, LEVEL3_AI_COUNT, state.map);

    if (state.player->inair && state.player->collidedBottom) {
        Mix_PlayChannel(-1, land2, 0);
        //Mix_Volume(0, MIX_MAX_VOLUME / 4);
        state.player->inair = false;
    }

    if (state.player->position.x >= 10 && state.enemy[1].activated == false) {
        state.enemy[1].isActive = true;
        state.enemy[1].activated = true;
    }

    if (state.player->position.x >= 16 && state.enemy[3].activated == false) {
        state.enemy[3].isActive = true;
        state.enemy[3].activated = true;
    }

    for (int i = 0; i < LEVEL3_AI_COUNT; i++) {
        if (state.enemy[i].isActive) {
            state.enemy[i].Update(deltaTime, state.player, nullptr, 0, state.map);
        }
    }


    if (state.player->position.x >= 19) {
        state.nextScene = 4;
    }
    if (state.player->position.y <= -8 ) {
        state.nextScene = 5;
    }

    if (state.player->hit) {
        state.player->life += 1;
        state.player->position = glm::vec3(1, -4, 0);
    }

}
void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_AI_COUNT; i++) {
        if (state.enemy[i].isActive) {
            state.enemy[i].Render(program);
        }
    }
}