#pragma once
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
#include "map.h"

enum EntityType{ PLAYER , PLATFORM, ENEMY};

enum AIType { WALKER, FOLLOWINRANGE, CHASER};
enum AIState { PATROL, WALKING, FOLLOW, PAUSE};

class Entity {
public:

    EntityType entityType;
    EntityType lastCollision;
    Entity* collidedwith;

    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;

    float width = 1;
    float height = 1;

    bool jump = false;
    bool inair = true;
    float jpower = 0;

    int life = 3;

    bool hit = false;

    GLuint textureID;

    glm::mat4 modelMatrix;

    bool isActive = true;
    bool collided = false;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedEnemy = false;

    //Enemy
    int Left = 0;
    int Right = 0;

    int turnAround;
    bool activated = false;

    Entity();

    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);

    void Update(float deltaTime, Entity *player, Entity* objects, int objectCount, Map *map);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    void AI(Entity* player);
    void AIWalker();
    void AIFOLLOWINRANGE(Entity* player);
    void AICHASER(Entity* player);
};
