#pragma once
#include <Component/SimpleScene.h>
#include <Core/Audio/irrKlang.h>
#include <deque>
#include <random>
#include "Camera.h"
#include "Tile.h"
#include "Player.h"

#define CURRENT_TILE 0
#define RED_TILE 1
#define SIMPLE_TILE 2
#define YELLOW_TILE 3
#define ORANGE_TILE 4
#define GREEN_TILE 5
#define WHITE_TILE 6
#define MAGENTA_TILE 7

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int shaderOption = 0);
	Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void Reset();
	void UpdateHighSpeedMode();
	void RenderHUD();
	void CheckGameOver(float deltaTimeSeconds);
	void CheckPowerUps();
	void UpdateTiles(float deltaTimeSeconds);
	void RenderPlayer(float deltaTimeSeconds);

protected:
	Lab::Camera* camera;
	glm::mat4 projectionMatrix;
	bool renderCameraTarget;
	float fov;
	irrklang::ISoundEngine* soundEngine;
	float speed = 0;
	float speedLimit = 16;
	std::deque<std::deque<Tile>> tiles;

	Player player;
	long long numOfTiles = 16;
	int numOfColumns = 3;
	bool isFirstPerson = false;
	int distBetweenTiles = 0;
	float deathTime = 0;
	float fuel = 1;
	float highSpeedStartTime = 0;
	float highSpeedStopTime = 0;
	float savedSpeed = 0;
	glm::vec3 playerInitPos = glm::vec3(0, 3, -4);
	int lives = 3;
	float fuelDrainSpeed = 500;
	int score = 0;
	int highScore = 0;
};
