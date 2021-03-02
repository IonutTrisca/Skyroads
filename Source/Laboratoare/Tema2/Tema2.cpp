#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	camera = new Lab::Camera();
	camera->Set(glm::vec3(0, 3, 0), glm::vec3(0, 1, -3.5f), glm::vec3(0, 1, 0));

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("tile");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "tile.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "hq_sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Shader* shader = new Shader("TileShader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/TileShader.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/TileShader.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("PlayerShader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/PlayerShader.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/PlayerShader.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("HUDShader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/HUDShader.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/HUDShader.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// mesh of a heart
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(0, -1,  0), glm::vec3(0, 1, 1)),
			VertexFormat(glm::vec3(-2, 1,  0), glm::vec3(1, 0, 1)),
			VertexFormat(glm::vec3(-1,  2,  0), glm::vec3(0, 0, 1)),
			VertexFormat(glm::vec3(0,  1,  0), glm::vec3(1, 0, 1)),
			VertexFormat(glm::vec3(1,  2,  0), glm::vec3(1, 0, 1)),
			VertexFormat(glm::vec3(2,  1,  0), glm::vec3(1, 0, 1))
		};

		vector<unsigned short> indices =
		{
			0, 3, 1,
			1, 3, 2,
			3, 5, 4,
			0, 5, 3
		};

		Mesh* heart = CreateMesh("heart", vertices, indices);
	}

	{
		soundEngine = irrklang::createIrrKlangDevice();

		if (!soundEngine)
		{
			cout << "Could not startup engine\n";
		}

		soundEngine->play2D("Audio/sr_menu.ogg", true, false, true)->setVolume(0.1);
	}


	fov = RADIANS(60);
	projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);

	deque<Tile> t1, t2, t3;
	tiles.push_back(t1);
	tiles.push_back(t2);
	tiles.push_back(t3);

	for (int i = 0; i < numOfTiles; i++) {
		tiles[0].push_back(Tile(glm::vec3(-2 - distBetweenTiles, 1, -i * 4), "tile", -1));
		tiles[1].push_back(Tile(glm::vec3(0, 1, -i * 4), "tile", 0));
		tiles[2].push_back(Tile(glm::vec3(2 + distBetweenTiles, 1, -i * 4), "tile", 1));
	}

	player = Player();
	player.position = playerInitPos;
	player.radius = 0.3;
}

void Tema2::Reset()
{
	for(int i = 0; i < numOfColumns; i++)
		tiles[i].clear();

	for (int i = 0; i < numOfTiles; i++) {
		tiles[0].push_back(Tile(glm::vec3(-2 - distBetweenTiles, 1, -i * 4), "tile", -1));
		tiles[1].push_back(Tile(glm::vec3(0, 1, -i * 4), "tile", 0));
		tiles[2].push_back(Tile(glm::vec3(2 + distBetweenTiles, 1, -i * 4), "tile", 1));
	}

	player.position = playerInitPos;
	player.velocity = 0;
	player.crashed = false;

	speed = 0;

	camera->Set(glm::vec3(0, 3, 0), glm::vec3(0, 1, -3.5f), glm::vec3(0, 1, 0));
	deathTime = 0;
	fuel = 1;

	highSpeedStartTime = 0;
	highSpeedStopTime = 0;
	lives = 3;
	score = 0;
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Crete the IBO and bind it
	unsigned int IBO = 0;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	return meshes[name];
}

void Tema2::UpdateHighSpeedMode()
{
	// check if high speed was activated
	if (highSpeedStopTime != 0)
		highSpeedStartTime = Engine::GetElapsedTime();

	if (highSpeedStartTime > highSpeedStopTime)
	{
		highSpeedStartTime = 0;
		highSpeedStopTime = 0;
		if (deathTime == 0)
			speed = savedSpeed;
	}
}

void Tema2::RenderHUD()
{
	// Render fuel gauge
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.05, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -19, -1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel, 1, 1));
		RenderMesh(meshes["box"], shaders["HUDShader"], modelMatrix, 1);

		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0 / fuel, 1, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.005, 1.1, 1));

		RenderMesh(meshes["box"], shaders["HUDShader"], modelMatrix, 0);
	}

	// Render speed gauge
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 1, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-19, 0, 0));

		if (speed == 0)
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.0001, 1));
		else
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1, glm::min(speed / speedLimit, 1.0f), 1));
		RenderMesh(meshes["box"], shaders["HUDShader"], modelMatrix, 2);

		if (speed == 0)
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1.0 / 0.0001, 1));
		else
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1.0 / glm::min(speed / speedLimit, 1.0f), 1));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1, 1.005, 1));
		RenderMesh(meshes["box"], shaders["HUDShader"], modelMatrix, 3);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1, 1.005, 1));
		RenderMesh(meshes["box"], shaders["HUDShader"], modelMatrix, 0);
	}

	// render fuel lives left
	{
		glm::mat4 modelMatrix = glm::mat4(1);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.93, 0.89, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.04, 0.04, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1 / 1.77, 1, 1));
		if (lives >= 1)
			RenderMesh(meshes["heart"], shaders["HUDShader"], modelMatrix);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5, 0, 0));
		if (lives >= 2)
			RenderMesh(meshes["heart"], shaders["HUDShader"], modelMatrix);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5, 0, 0));
		if (lives >= 3)
			RenderMesh(meshes["heart"], shaders["HUDShader"], modelMatrix);
	}
}

void Tema2::CheckGameOver(float deltaTimeSeconds)
{
	// update fuel
	if (fuel > 0)
		fuel -= deltaTimeSeconds * speed / fuelDrainSpeed;
	else
	{
		if (lives > 1)
			fuel = 1;
		else
			fuel = 0;

		lives--;
	}

	// check red tile or player left the playable area
	if (deathTime == 0)
		if (player.position.y < 0.9 ||
			(tiles[player.getColumn()][1].wasTouched && tiles[player.getColumn()][1].shaderType == RED_TILE) ||
			fuel == 0) {
			deathTime = Engine::GetElapsedTime();
			soundEngine->play2D("Audio/death.ogg", false, false, true)->setVolume(0.8);

			if (highSpeedStartTime != 0) {
				speed = savedSpeed;
			}

			lives = 0;
			cout << "Scorul tau este: " << score << "\n";
			if (score > highScore)
				highScore = score;
			cout << "Highscorul este: " << highScore << "\n";
		}

	// reduce speed to end the game
	if (deathTime > 0)
		if (speed > 0)
			speed -= deltaTimeSeconds * 10;
		else
			Reset();
}

void Tema2::CheckPowerUps()
{
	// check for powerups
	if (tiles[player.getColumn()][1].wasTouched && !tiles[player.getColumn()][1].isPowerActive)
	{
		if (tiles[player.getColumn()][1].shaderType == YELLOW_TILE)
		{
			fuel -= fuel / 3.0;
			tiles[player.getColumn()][1].isPowerActive = true;
		}

		if (tiles[player.getColumn()][1].shaderType == ORANGE_TILE && deathTime == 0)
		{
			if (highSpeedStartTime == 0)
				savedSpeed = speed;

			highSpeedStartTime = Engine::GetElapsedTime();
			highSpeedStopTime = Engine::GetElapsedTime() + 3;
			speed = speedLimit * 2;

			tiles[player.getColumn()][1].isPowerActive = true;
		}

		if (tiles[player.getColumn()][1].shaderType == GREEN_TILE)
		{
			fuel = glm::min(fuel + fuel / 2.0, 1.0);
			tiles[player.getColumn()][1].isPowerActive = true;
		}

		if (tiles[player.getColumn()][1].shaderType == WHITE_TILE)
		{
			lives++;
			lives = lives > 3 ? 3 : lives;
			tiles[player.getColumn()][1].isPowerActive = true;
		}

		if (tiles[player.getColumn()][1].shaderType == MAGENTA_TILE)
		{
			lives--;
			tiles[player.getColumn()][1].isPowerActive = true;
		}
	}
}

void Tema2::UpdateTiles(float deltaTimeSeconds)
{
	// move and render tiles
	bool outside = false;
	int vanishPos = 2;

	for (int j = 0; j < numOfColumns; j++)
		for (int i = 0; i < numOfTiles; i++)
		{
			if (!player.crashed)
				tiles[j][i].move(deltaTimeSeconds * speed);

			tiles[j][i].isCurrent = (i == 1 && player.testCollision(tiles[j][i]));

			RenderMesh(meshes[tiles[j][i].type], shaders[tiles[j][i].shader], tiles[j][i].modelMatrix,
				tiles[j][i].wasTouched ? CURRENT_TILE : tiles[j][i].shaderType);

			if (tiles[j][i].position.z > vanishPos)
				outside = true;
		}

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator

	// delete out of view tiles
	if (outside)
	{
		score++;
		float offset = tiles[0][0].position.z - vanishPos;
		for (int i = 0; i < numOfColumns; i++)
			tiles[i].pop_front();

		std::uniform_int_distribution<> distr(0, 1); // define the range
		int a;

		a = distr(gen);
		tiles[0].push_back(Tile(glm::vec3(-2 - distBetweenTiles, 1, (-4 * numOfTiles) + vanishPos + offset), a ? "tile" : "", -1));
		a = distr(gen);
		tiles[1].push_back(Tile(glm::vec3(0, 1, (-4 * numOfTiles) + vanishPos + offset), a ? "tile" : "", 0));
		a = distr(gen);
		tiles[2].push_back(Tile(glm::vec3(2 + distBetweenTiles, 1, (-4 * numOfTiles) + vanishPos + offset), a ? "tile" : "", 1));

		// make sure there is at least one tile per row
		int numTilesRow = 0;

		for (int i = 0; i < numOfColumns; i++)
			if (tiles[i][numOfTiles - 1].type == "tile")
				numTilesRow++;

		if (numTilesRow == 0)
		{
			distr = std::uniform_int_distribution<>(0, 2);
			tiles[distr(gen)][numOfTiles - 1].type = "tile";
			numTilesRow++;
		}

		distr = std::uniform_int_distribution<>(1, 40);

		for (int i = 0; i < numOfColumns; i++)
		{
			if (numTilesRow >= 2)
			{
				a = distr(gen);
				if (tiles[i][numOfTiles - 1].type != "")
					tiles[i][numOfTiles - 1].shaderType = a > 7 ? SIMPLE_TILE : a;
			}
			else
			{
				distr = std::uniform_int_distribution<>(2, 40);
				a = distr(gen);
				if (tiles[i][numOfTiles - 1].type != "")
					tiles[i][numOfTiles - 1].shaderType = a > 7 ? SIMPLE_TILE : a;
			}
		}
	}
}

void Tema2::RenderPlayer(float deltaTimeSeconds)
{
	// render player, and apply gravity
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, player.position);
	modelMatrix = glm::rotate(modelMatrix, deltaTimeSeconds * speed, glm::vec3(1, 0, 0));

	if (!player.crashed)
	{
		if (player.applyGravity(deltaTimeSeconds, tiles[player.getColumn()][1]))
		{
			deathTime = Engine::GetElapsedTime();
			soundEngine->play2D("Audio/crash.ogg", false, false, true)->setVolume(0.1);
			if (highSpeedStartTime != 0) {
				speed = savedSpeed;
			}

			lives = 0;
			cout << "Scorul tau este: " << score << "\n";
			if (score > highScore)
				highScore = score;
			cout << "Highscorul este: " << highScore << "\n";
		}
	}
	else
		modelMatrix = glm::scale(modelMatrix, glm::vec3(speed / speedLimit, speed / speedLimit, speed / speedLimit));

	if (isFirstPerson)
		camera->Set(glm::vec3(player.position.x, 2 + player.position.y - 1.1, player.position.z),
			glm::vec3(player.position.x, player.position.y, player.position.z - 3.5f),
			glm::vec3(0, 1, 0));

	std::uniform_int_distribution<> distr(1, 20);

	if (!isFirstPerson)
		if (((tiles[player.getColumn()][1].shaderType != SIMPLE_TILE && tiles[player.getColumn()][1].wasTouched) ||
			highSpeedStartTime != 0) || player.crashed || player.position.y < 0.9)
		{
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5, 0.5));
			RenderMesh(meshes["sphere"], shaders["PlayerShader"], modelMatrix, distr(gen));
		}
		else
		{
			RenderMesh(meshes["sphere"], shaders["PlayerShader"], modelMatrix);
		}
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
	UpdateHighSpeedMode();
	RenderHUD();
	CheckGameOver(deltaTimeSeconds);
	CheckPowerUps();
	UpdateTiles(deltaTimeSeconds);
	RenderPlayer(deltaTimeSeconds);
}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int shaderOption)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (shader->GetName() == shaders["TileShader"]->GetName())
		glUniform1i(glGetUniformLocation(shader->program, "type"), shaderOption);

	if (shader->GetName() == shaders["HUDShader"]->GetName())
		glUniform1i(glGetUniformLocation(shader->program, "type"), shaderOption);

	if (shader->GetName() == shaders["PlayerShader"]->GetName())
		glUniform1i(glGetUniformLocation(shader->program, "is_powered_up"), shaderOption);
	
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) && deathTime == 0 && highSpeedStartTime == 0 && !player.crashed)
	{
		if (speed >= speedLimit)
			speed = speedLimit;
		else
			speed += deltaTime * 10;
	}

	if (window->KeyHold(GLFW_KEY_S) && deathTime == 0 && highSpeedStartTime == 0 && !player.crashed)
	{
		if (speed <= 0)
			speed = 0;
		else
			speed -= deltaTime * 10;
	}

	if (window->KeyHold(GLFW_KEY_A) && !player.crashed) 
	{

		player.position.x -= deltaTime * 5;
	}

	if (window->KeyHold(GLFW_KEY_D) && !player.crashed) 
	{
		player.position.x += deltaTime * 5;
	}

	if (window->KeyHold(GLFW_KEY_SPACE) && player.velocity == 0)
	{
		player.velocity = -0.07;
	}
	
}

void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_C)
	{
		if (isFirstPerson)
		{
			camera->Set(glm::vec3(0, 3, 0), glm::vec3(0, 1, -3.5f), glm::vec3(0, 1, 0));
		}

		isFirstPerson = !isFirstPerson;
	}

	if (key == GLFW_KEY_R)
	{
		Reset();
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
