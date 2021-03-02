#include "Tile.h"
#include <string>

Tile::Tile(glm::vec3 position, std::string type, int column)
{
	this->position = position;
	modelMatrix = glm::translate(glm::mat4(1), position);
	this->type = type;
	this->column = column;
}

Tile::~Tile()
{
}

void Tile::move(float movement)
{
	position.z += movement;
	modelMatrix = glm::translate(glm::mat4(1), position);
}