#include "Entity.h"

Entity::Entity(){
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    entityType;
    lastCollision;
    collidedwith = nullptr;
    inair = false;

    isActive = true;

    textureID;

    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++){
        Entity* object = &objects[i];
        //if (object->entityType == ENEMY) return;
        if (CheckCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (penetrationY < 0.9 && penetrationY > 0.2) {
                penetrationY = .05;
            }
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++){
        Entity* object = &objects[i];
        if (object->entityType == ENEMY) return;
        if (CheckCollision(object)){
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
            }
        }
    }
}

void Entity::CheckEnemyY(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];
        if (CheckCollision(object)) {
            collidedwith = object;
            if (position.y > object->position.y) {
                collidedBottom = true;
                lastCollision = object->entityType;
            }
            else if (position.y < object->position.y) {
                collidedTop = true;
                lastCollision = object->entityType;
            }
        }
    }
}

void Entity::CheckEnemyX(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];
        if (CheckCollision(object)) {
            collidedwith = object;
            if (position.x > object->position.x) {
                collidedLeft = true;
                lastCollision = object->entityType;
            }
            else if (position.x < object->position.x) {
                collidedRight = true;
                lastCollision = object->entityType;
            }
        }
    }
}

void Entity::AIWalker() {
    switch (aiState) {
    case WALKING:
        if (position.x > 3.5) {
            movement = glm::vec3(-1, 0, 0);
        }
        else if (position.x < 2) {
            movement = glm::vec3(1, 0, 0);
        }
        break;
    }
}

void Entity::AIFOLLOWINRANGE(Entity* player) {
    switch (aiState) {
    case PATROL:
        if (glm::distance(position, player->position) < 2.5f) {
            aiState = WALKING;
        }
        else {
            if (position.x > 0) {
                movement = glm::vec3(-1, 0, 0);
            }
            else if (position.x < -4) {
                movement = glm::vec3(1, 0, 0);
            }
        }
        break;

    case WALKING:
        if (glm::distance(position, player->position) > 2.5f) {
            aiState = PATROL;
        }
        else {
            if (player->position.x < position.x) {
                movement = glm::vec3(-2, 0, 0);
            }
            if (player->position.x > position.x) {
                movement = glm::vec3(2, 0, 0);
            }
        }
        break;
    }
}

void Entity::AICHASER(Entity* player) {
    switch (aiType) {
    case FOLLOW:
        if (glm::distance(position, player->position) < 2.0f) {
            aiState = PAUSE;
        }
        else {
            if (player->position.x < position.x) {
                movement = glm::vec3(-1.5, 0, 0);
            }
            if (player->position.x > position.x) {
                movement = glm::vec3(1.5, 0, 0);
            }
        }
        break;
    case PAUSE:
        if (glm::distance(position, player->position) > 2.0f) {
            aiState = FOLLOW;
        }
    }
}

void Entity::AI(Entity* player) {
    switch (aiType) {
    case WALKER:
        AIWalker();
        break;
    case FOLLOWINRANGE:
        AIFOLLOWINRANGE(player);
        break;
    case CHASER:
        AICHASER(player);
        break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* object, int objectCount) {
    if (isActive == false) return;

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    inair = false;

    if (entityType == ENEMY) {
        AI(player);
    }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    if (jump) {
        jump = false;

        velocity.y += jpower;
    }

    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime;
    position.x += velocity.x * deltaTime;

    CheckEnemyY(object, objectCount);
    CheckEnemyX(object, objectCount);
    if ((collidedBottom || collidedTop) && (collidedLeft || collidedRight)) {
        inair = true;
    }
    CheckCollisionsY(object, objectCount);
    CheckCollisionsX(object, objectCount);

    if ((collidedLeft || collidedRight) && !collidedBottom && collidedwith->entityType == ENEMY) {
        isActive = false;
    }

    if (collidedBottom && lastCollision == ENEMY) {
        collidedwith->isActive = false;
    }
    

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index){
    if (isActive == false) return;

    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) return;

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
