#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 26

#define LANDING_COUNT 2

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* landing_pad;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 4, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -0.02f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("rocket_ship.png");

    state.player->height = 0.7f;
    state.player->width = 0.7f;

    state.landing_pad = new Entity[LANDING_COUNT];
    state.landing_pad->entityType = LANDING_PAD;
    GLuint padTextureID = LoadTexture("objective.png");

    state.landing_pad[0].textureID = padTextureID;
    state.landing_pad[0].position = glm::vec3(1, -3.25f, 0);

    state.landing_pad[1].textureID = padTextureID;
    state.landing_pad[1].position = glm::vec3(2, -3.25f, 0);

    for (int i = 0; i < LANDING_COUNT; i++) {
        state.landing_pad[i].Update(0, NULL, 0);
    }
    
    state.platforms = new Entity[PLATFORM_COUNT];
    state.platforms->entityType = WALL;
    GLuint platformTextureID = LoadTexture("TopLine.png");
    GLuint platform2TextureID = LoadTexture("RightLine.png");
    GLuint platform3TextureID = LoadTexture("TopBottomLine.png");
    GLuint platform4TextureID = LoadTexture("LeftU.png");
    GLuint platform5TextureID = LoadTexture("RightU.png");
    GLuint platform6TextureID = LoadTexture("LeftLine.png");
    GLuint platform7TextureID = LoadTexture("LeftL.png");
    GLuint platform8TextureID = LoadTexture("RightL.png");

    // Platforms on the bottom 

    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].position = glm::vec3(4.5, -3.25f, 0);

    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].position = glm::vec3(4, -3.25f, 0);

    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].position = glm::vec3(3, -3.25f, 0);

    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].position = glm::vec3(0, -3.25f, 0);

    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].position = glm::vec3(-1, -3.25f, 0);

    state.platforms[5].textureID = platformTextureID;
    state.platforms[5].position = glm::vec3(-2, -3.25f, 0);

    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].position = glm::vec3(-3, -3.25f, 0);

    state.platforms[7].textureID = platformTextureID;
    state.platforms[7].position = glm::vec3(-4, -3.25f, 0);

    state.platforms[8].textureID = platformTextureID;
    state.platforms[8].position = glm::vec3(-5, -3.25f, 0);

    state.platforms[9].textureID = platform6TextureID;// start going upwards right
    state.platforms[9].position = glm::vec3(4.50, -2.25f, 0);

    state.platforms[10].textureID = platform6TextureID;
    state.platforms[10].position = glm::vec3(4.50, -1.25f, 0);

    state.platforms[11].textureID = platform6TextureID;
    state.platforms[11].position = glm::vec3(4.50, -0.25f, 0);

    state.platforms[12].textureID = platform6TextureID;
    state.platforms[12].position = glm::vec3(4.50, 0.75f, 0);

    state.platforms[13].textureID = platform6TextureID;
    state.platforms[13].position = glm::vec3(4.50, 1.75f, 0);

    state.platforms[14].textureID = platform8TextureID;
    state.platforms[14].position = glm::vec3(4.50, 2.75f, 0);

    state.platforms[15].textureID = platform2TextureID;// start going upwards left
    state.platforms[15].position = glm::vec3(-4.50, -2.25f, 0);

    state.platforms[16].textureID = platform2TextureID;
    state.platforms[16].position = glm::vec3(-4.50, -1.25f, 0);

    state.platforms[17].textureID = platform2TextureID;
    state.platforms[17].position = glm::vec3(-4.50, -0.25f, 0);

    state.platforms[18].textureID = platform2TextureID;
    state.platforms[18].position = glm::vec3(-4.50, 0.75f, 0);

    state.platforms[19].textureID = platform3TextureID; // left floating ledge
    state.platforms[19].position = glm::vec3(-3.5, 1.75f, 0);

    state.platforms[20].textureID = platform5TextureID; 
    state.platforms[20].position = glm::vec3(-2.5, 1.75f, 0);

    state.platforms[21].textureID = platform4TextureID; // middle floating ledge
    state.platforms[21].position = glm::vec3(0.5, 0.75f, 0);

    state.platforms[22].textureID = platform3TextureID; 
    state.platforms[22].position = glm::vec3(1.5, 0.75f, 0);

    state.platforms[23].textureID = platform5TextureID;
    state.platforms[23].position = glm::vec3(2.5, 0.75f, 0);

    state.platforms[24].textureID = platform2TextureID;
    state.platforms[24].position = glm::vec3(-4.50, 1.75f, 0);

    state.platforms[25].textureID = platform7TextureID;
    state.platforms[25].position = glm::vec3(-4.50, 2.75f, 0);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
}

void ProcessInput() {

    if (state.player->collided) {
        state.player->isActive = false;
    }

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->acceleration.x = -0.1f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->acceleration.x = 0.1f;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.016666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.landing_pad, LANDING_COUNT);
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < LANDING_COUNT; i++) {
        state.landing_pad[i].Render(&program);
    }

    state.player->Render(&program);

    if (state.player->isActive == false) {
        if (state.player->lastCollision == WALL) {
            DrawText(&program, LoadTexture("font1.png"), "Mission Fail", 1, -0.5f, glm::vec3(-2.5, 2.7, 0));
        }
        else if (state.player->lastCollision == LANDING_PAD){
            DrawText(&program, LoadTexture("font1.png"), "Mission Success", 1, -0.6f, glm::vec3(-3, 2.7, 0));
        }
    }

    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
