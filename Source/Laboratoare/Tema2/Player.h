#pragma once
#include <Core/Engine.h>
#include <cmath>
#include <Core/Audio/irrKlang.h>
#include <iostream>

#include "Tile.h"

class Player
{
public:
	Player();
	Player(glm::vec3 position);
	~Player();

	bool applyGravity(float deltaTimeSeconds, Tile& tile);
	bool testCollision(Tile& tile);
	int getColumn();

	glm::vec3 position;
	float velocity = 0;
	float radius;
	bool crashed = false;
};