#pragma once
#include "Scene.h"
class Gameover : public Scene {

public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};