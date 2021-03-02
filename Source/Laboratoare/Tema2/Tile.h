#pragma once
#include <Core/Engine.h>

class Tile 
{
public:
	Tile(glm::vec3 position, std::string type, int column);
	~Tile();

	void move(float movement);

	glm::vec3 position;
	std::string type;
	glm::mat4 modelMatrix;
	int column;
	std::string shader = "TileShader";
	int shaderType = 2;
	bool isCurrent = false;
	bool wasTouched = false;
	bool isPowerActive = false;
};