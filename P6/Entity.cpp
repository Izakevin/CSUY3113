#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(0);
    scale = glm::vec3(1);

    isActive = true;
    noise = false;

    lives = 3;
    shots = 3;
    
    speed = 1.0f;

    billboard = false;

    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;
}

bool Entity::CheckCollision(Entity* other){
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;

    return false;
}


void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* enemies, int enemiesCount){
    glm::vec3 previousPosition = position;

    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));

        velocity.z = cos(glm::radians(rotation.y)) * -1.0f * speed;
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f * speed;
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    if (entityType == PLAYER) {
        for (int i = 0; i < objectCount; i++) {
            if (objects[i].entityType == FLOOR) continue;
            if (CheckCollision(&objects[i])) {
                position = previousPosition;    
                break;
            }
        }
        for (int i = 0; i < enemiesCount; i++) {
            if (CheckCollision(&enemies[i]) && enemies[i].isActive) {
                enemies[i].isActive = false;
                lives -= 1;
                break;
            }
        }
    }

    if (entityType == AMMO) {
        for (int i = 0; i < objectCount; i++) {
            if (CheckCollision(&objects[i])) {
                isActive = false;
                break;
            }
        }
        for (int i = 0; i < enemiesCount; i++) {
            if (CheckCollision(&enemies[i]) && enemies[i].isActive) {
                enemies[i].isActive = false;
                isActive = false;
                break;
            }
        }
    }

    if (lives == 0) {
        isActive = false;
    }

    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Entity::Render(ShaderProgram *program) {

    program->SetModelMatrix(modelMatrix);
        
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    if (billboard) {
        DrawBillboard(program);
    }
    else {
        mesh->Render(program);
    }
}

void Entity::DrawBillboard(ShaderProgram* program) {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

