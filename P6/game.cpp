#include "game.h"

#define OBJECT_COUNT 6
#define ENEMY_COUNT 6
#define AMMO_COUNT 3

bool all_alive = true;;
int rounds = 1;

GLuint iconTextureID;
GLuint crosshairTextureID;

Mix_Music* music;
Mix_Chunk* fireball;

void game::Initialize() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    state.uiViewMatrix = glm::mat4(1.0);
    state.uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);

    state.num_ammo = AMMO_COUNT;
    state.num_alive = ENEMY_COUNT;

    state.nextScene = -1;

    state.fontTextureID = Util::LoadTexture("font1.png");
    iconTextureID = Util::LoadTexture("lava-icon.png");
    crosshairTextureID = Util::LoadTexture("crosshair.png");

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(24, 0.75f, 24);
    state.player->rotation = glm::vec3(0, 45, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
    state.player->shots = AMMO_COUNT;

    state.objects = new Entity[OBJECT_COUNT];

    GLuint floorTextureID = Util::LoadTexture("darkground.jpg");

    Mesh* cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj", 10);

    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0, -0.25f, 0);
    state.objects[0].rotation = glm::vec3(0, 0, 0);
    state.objects[0].acceleration = glm::vec3(0, 0, 0);
    state.objects[0].scale = glm::vec3(50, 0.15f, 50);
    state.objects[0].entityType = FLOOR;

    state.objects[1].textureID = floorTextureID;
    state.objects[1].mesh = cubeMesh;
    state.objects[1].position = glm::vec3(0, 4, 0);
    state.objects[1].rotation = glm::vec3(0, 0, 0);
    state.objects[1].acceleration = glm::vec3(0, 0, 0);
    state.objects[1].scale = glm::vec3(50, 0.15f, 50);
    state.objects[1].entityType = FLOOR;

    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 3);

    GLuint wallTextureID = Util::LoadTexture("redwall.png");

    state.objects[2].textureID = wallTextureID;
    state.objects[2].mesh = wallMesh;
    state.objects[2].position = glm::vec3(25, 2, 0);
    state.objects[2].rotation = glm::vec3(0, 0, 0);
    state.objects[2].acceleration = glm::vec3(0, 0, 0);
    state.objects[2].scale = glm::vec3(0.15f, 4.5, 50);
    state.objects[2].entityType = WALL;
    state.objects[2].width = 0.15f;
    state.objects[2].height = 4.5;
    state.objects[2].depth = 50;

    state.objects[3].textureID = wallTextureID;
    state.objects[3].mesh = wallMesh;
    state.objects[3].position = glm::vec3(-25, 2, 0);
    state.objects[3].rotation = glm::vec3(0, 0, 0);
    state.objects[3].acceleration = glm::vec3(0, 0, 0);
    state.objects[3].scale = glm::vec3(0.15f, 4.5, 50);
    state.objects[3].entityType = WALL;
    state.objects[3].width = 0.15f;
    state.objects[3].height = 4.5;
    state.objects[3].depth = 50;

    state.objects[4].textureID = wallTextureID;
    state.objects[4].mesh = wallMesh;
    state.objects[4].position = glm::vec3(0, 2, 25);
    state.objects[4].rotation = glm::vec3(0, 0, 0);
    state.objects[4].acceleration = glm::vec3(0, 0, 0);
    state.objects[4].scale = glm::vec3(50, 4.5, 0.15f);
    state.objects[4].entityType = WALL;
    state.objects[4].width = 50;
    state.objects[4].height = 4.5;
    state.objects[4].depth = 0.15f;

    state.objects[5].textureID = wallTextureID;
    state.objects[5].mesh = wallMesh;
    state.objects[5].position = glm::vec3(0, 2, -25);
    state.objects[5].rotation = glm::vec3(0, 0, 0);
    state.objects[5].acceleration = glm::vec3(0, 0, 0);
    state.objects[5].scale = glm::vec3(50, 4.5, 0.15f);
    state.objects[5].entityType = WALL;
    state.objects[5].width = 50;
    state.objects[5].height = 4.5;
    state.objects[5].depth = 0.15f;

    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("DG.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(rand() % 25 - 10, 0.5f, rand() % 25 - 10);
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
    }

    state.ammo = new Entity[AMMO_COUNT];

    GLuint ammoTextureID = Util::LoadTexture("lava.png");
    Mesh* ammoMesh = new Mesh();
    ammoMesh->LoadOBJ("cube.obj", 1);

    for (int i = 0; i < AMMO_COUNT; i++) {
        state.ammo[i].textureID = ammoTextureID;
        state.ammo[i].mesh = ammoMesh;
        state.ammo[i].position = glm::vec3(0, 0, 0);
        state.ammo[i].rotation = glm::vec3(0, 0, 0);
        state.ammo[i].acceleration = glm::vec3(0, 0, 0);
        state.ammo[i].isActive = false;
        state.ammo[i].scale = glm::vec3(0.1f, 0.1f, 0.1f);
        state.ammo[i].entityType = AMMO;
        state.ammo[i].width = 0.1f;
        state.ammo[i].height = 0.1f;
        state.ammo[i].depth = 0.1f;
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("ready aim fire.mp3");
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_PlayMusic(music, -1);

    fireball = Mix_LoadWAV("fireball.wav");
    Mix_Volume(1, MIX_MAX_VOLUME);
}

void game::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);

    state.player->shots = AMMO_COUNT;
    for (int i = 0; i < AMMO_COUNT; i++) {
        if (state.ammo[i].noise) {
            Mix_PlayChannel(-1, fireball, 0);
            Mix_Volume(0, MIX_MAX_VOLUME);
            state.ammo[i].noise = false;
        }
        if (state.ammo[i].isActive) {
            state.player->shots -= 1;
            state.ammo[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
    }
    if (!all_alive) {
        all_alive = true;
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (!state.enemies[i].isActive) {
                state.enemies[i].isActive = true;
                state.enemies[i].position = glm::vec3(rand() % 30 - 10, 0.5f, rand() % 30 - 10);
                state.enemies[i].speed += 0.275f;
            }
        }
        rounds += 1;
    }

    if (rounds >= 13) {
        Mix_HaltMusic();
        Mix_FreeChunk(fireball);
        Mix_FreeMusic(music);
        state.nextScene = 2;
    }

    for (int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    }

    all_alive = false;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].isActive) {
            all_alive = true;
            state.enemies[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
    }

    if (!state.player->isActive) {
        Mix_HaltMusic();
        Mix_FreeChunk(fireball);
        Mix_FreeMusic(music);
        state.nextScene = 3;
    }
}

void game::Render(ShaderProgram* program) {
    for (int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].isActive) {
            state.enemies[i].Render(program);
        }
    }

    for (int i = 0; i < AMMO_COUNT; i++) {
        if (state.ammo[i].isActive) {
            state.ammo[i].Render(program);
        }
    }

    program->SetProjectionMatrix(state.uiProjectionMatrix);
    program->SetViewMatrix(state.uiViewMatrix);

    Util::DrawText(program, state.fontTextureID, "Lives: " + std::to_string(state.player->lives), 0.5, -0.3f, glm::vec3(-6, 3.2, 0));
    Util::DrawText(program, state.fontTextureID, "Rounds: " + std::to_string(rounds), 0.5, -0.3f, glm::vec3(0, 3.2, 0));
    for (int i = 0; i < state.player->shots; i++) {
        Util::DrawIcon(program, iconTextureID, glm::vec3(4 + (i * 0.75f), 3.2, 0));
    }

    Util::DrawIcon(program, crosshairTextureID, glm::vec3(0, 0, 1));
}