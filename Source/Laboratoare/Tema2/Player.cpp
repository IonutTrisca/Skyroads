#include "Player.h"

Player::Player(glm::vec3 position)
{
	this->position = position;
}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::applyGravity(float deltaTimeSeconds, Tile& tile)
{
	position.y -= velocity;
	bool collided = testCollision(tile);

	// test collision only if a tile exists
	if (tile.type != "")
		if (collided)
			if ((float)position.y - radius < 0.1f + tile.position.y && position.y - radius > tile.position.y - velocity)
			{
				position.y = 0.1 + tile.position.y + radius;
				velocity = 0;
			}
			else
			{
				// update the crashed player
				crashed = true;
				return true;
			}
		else
			velocity += deltaTimeSeconds / 5;
		
	else
		velocity += deltaTimeSeconds / 5;

	return false;
}

// get the column of the player
int Player::getColumn()
{
	if (position.x < -1)
		return 0;
	if (position.x > 1)
		return 2;
	return 1;
}

// detect a collision between a give tile and the player
bool Player::testCollision(Tile &tile)
{
	float minX = -1 + tile.position.x, minY = -0.1 + tile.position.y, minZ = tile.position.z - 2;
	float maxX = 1 + tile.position.x, maxY = 0.1 + tile.position.y, maxZ = tile.position.z + 2;

	float x = std::max(minX, std::min(position.x, maxX));
	float y = std::max(minY, std::min(position.y, maxY));
	float z = std::max(minZ, std::min(position.z, maxZ));

	float distance = std::sqrt(	(x - position.x) * (x - position.x) +
								(y - position.y) * (y - position.y) +
								(z - position.z) * (z - position.z));

	if (distance < radius)
		tile.wasTouched = true;

	return distance < radius;
}