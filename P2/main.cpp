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

#define STB_IMAGE_IMPLEMENTATION // 
#include "stb_image.h" //

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, player1Matrix,player2Matrix,ballMatrix, projectionMatrix;

glm::vec3 player1_position = glm::vec3(-4.75, 0, 0);
glm::vec3 player1_movement = glm::vec3(0, 0, 0);

glm::vec3 player2_position = glm::vec3(4.75, 0, 0);
glm::vec3 player2_movement = glm::vec3(0, 0, 0);

glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(1, 1, 0);

float player_width = 0.5f;
float player_height = 2.0f;

float ball_width = 0.5f;
float ball_height = 0.5f;

float player_speed = 2.0f;
float ball_speed = 1.0f;

GLuint player1TextureID;
GLuint player2TextureID;
GLuint ballTextureID;

GLuint LoadTexture(const char* filePath) { //takes a file path and loads the image texture id and puts it on the video card
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

SDL_Joystick * playerTwoController;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    playerTwoController = SDL_JoystickOpen(0);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    player1TextureID = LoadTexture("bpaddle.png");
    player2TextureID = LoadTexture("rpaddle.png");
    ballTextureID = LoadTexture("bpaddle.png");
    SDL_JoystickClose(playerTwoController);
}

void ProcessInput() {
    player1_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_JOYAXISMOTION:
            event.jaxis.which = 0;
            event.jaxis.axis = 4;
            event.jaxis.value = 32767;
            break;
        case SDL_JOYBUTTONDOWN:
            event.jbutton.which = 0;
            event.jbutton.button = 12;
            player2_movement.y = -1.0f;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            case SDLK_w:
                player1_movement.y = 1.0f;
                break;

            case SDLK_s:
                player1_movement.y = -1.0f;
                break;

            case SDLK_UP:
                player2_movement.y = 1.0f;
                break;

            case SDLK_DOWN:
                player2_movement.y = -1.0f;
                break;

            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    //double axisIndex = 0;
    //double axisIndex = 0;
   // SDL_JoystickGetAxis(playerTwoController, axisIndex);
    //SDL_JoystickGetButton(playerTwoController, axisIndex);
    if (keys[SDL_SCANCODE_W]) {
        player1_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        player1_movement.y = -1.0f;
    }

    if (glm::length(player1_movement) > 1.0f) {
        player1_movement = glm::normalize(player1_movement);
    }

    if (keys[SDL_SCANCODE_UP]) {
        player2_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        player2_movement.y = -1.0f;
    }

    if (glm::length(player2_movement) > 1.0f) {
        player2_movement = glm::normalize(player2_movement);
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f; //SDL_GetTicks calculates the amount of miliseconds since SDL started 
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    player1_position += player1_movement * player_speed * deltaTime;
    player2_position += player2_movement * player_speed * deltaTime;

    ball_position += ball_movement * ball_speed * deltaTime ;

    float xdist = fabs(player2_position[0] - ball_position[0]) - ((ball_width + player_width) / 2.0f);
    float ydist = fabs(player2_position[1] - ball_position[1]) - ((ball_height + player_height) / 2.0f);

    float xdist1 = fabs(player1_position[0] - ball_position[0]) - ((ball_width + player_width) / 2.0f);
    float ydist1 = fabs(player1_position[1] - ball_position[1]) - ((ball_height + player_height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        ball_movement[0] *= -1;
        ball_speed += .01;
    }

    if (xdist1 < 0 && ydist1 < 0) {
        ball_movement[0] *= -1;
        ball_speed += .01;
    }

    if (ball_position[1] > 3.5) {
        ball_position[1] = 3.5;
        ball_movement[1] *= -1;
    }
    else if (ball_position[1] < -3.5) {
        ball_position[1] = -3.5;
        ball_movement[1] = 1;
    }

    if (player1_position[1] > 2.8f) {
        player1_position[1] = 2.8f;
    }
    else if (player1_position[1] < -2.8f) {
        player1_position[1] = -2.8f;
    }

    if (player2_position[1] > 2.8f) {
        player2_position[1] = 2.8f;
    }
    else if (player2_position[1] < -2.8f) {
        player2_position[1] = -2.8f;
    }

    if (ball_position[0] > 5) {
        gameIsRunning = false;
    }
    else if (ball_position[0] < -5) {
        gameIsRunning = false;
    }

    player1Matrix = glm::mat4(1.0f);
    player1Matrix = glm::translate(player1Matrix, player1_position);

    player2Matrix = glm::mat4(1.0f);
    player2Matrix = glm::translate(player2Matrix, player2_position);

    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetModelMatrix(player1Matrix);
    float vertices[] = { -0.25, -1.0, 0.25, -1.0, 0.25, 1.0, -0.25, -1.0, 0.25, 1.0, -0.25, 1.0 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, player1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(player2Matrix);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, player2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(ballMatrix);
    float bvertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    //{ -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    float btexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bvertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, btexCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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
