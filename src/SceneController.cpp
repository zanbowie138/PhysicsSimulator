#include "SceneController.h"

SceneController::SceneController(GLFWwindow* window, const GLuint screenWidth, const GLuint screenHeight):
	cam(screenWidth, screenHeight, glm::vec3(0.0f, 1.0f, 7.0f)), window(window)
{
	cam.UpdateMatrix(45.0f, 0.1f, 100.0f);

	UniformBlock ub = {};
	ub.camMatrix = cam.cameraMatrix;
	ub.camPos = glm::vec4(cam.position,1.0);
	ub.lightPos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	ub.lightColor = glm::vec4(1.0,1.0,1.0,1.0);
	char ub_char[sizeof(ub)];
	memcpy(ub_char, &ub, sizeof(ub));



	Shader defaultShader("default.vert", "default.frag");
	shaders.emplace_back(std::move(std::make_unique<Shader>(defaultShader)));
	Shader flatShader("flat.vert", "flat.frag");
	shaders.emplace_back(std::move(std::make_unique<Shader>(flatShader)));
	Shader basicShader("basic.vert", "basic.frag");
	shaders.emplace_back(std::move(std::make_unique<Shader>(basicShader)));

	UBO.Bind();
	UBO.AllocBuffer(112, GL_DYNAMIC_DRAW);

	for (auto&& s : shaders)
	{
		UBO.BindShader(*s, "Camera", 0);
	}
	UBO.BindShader(defaultShader, "Lighting", 1);
	UBO.BindShader(flatShader, "Lighting", 1);

	UBO.BindUniformRange(0, 0, sizeof(glm::mat4));
	UBO.BindUniformRange(1, 64, 48);


	UBO.EditBuffer(ub_char, sizeof(ub));
	UBO.Unbind();



	// Wood floor setup
	Vertex board_vertexes[] =
	{
		Vertex{glm::vec3(-1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0, 3.0)},
		Vertex{glm::vec3(-1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0, 3.0)}
	};
	GLuint board_indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	// Texture data
	Texture textures[]
	{
		Texture("planks.png", "diffuse", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png", "specular", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector<Vertex> verts(board_vertexes, board_vertexes + sizeof(board_vertexes) / sizeof(Vertex));
	std::vector<GLuint> ind(board_indices, board_indices + sizeof(board_indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh floor(verts, ind, tex);
	floor.scale = 10.0f;
	floor.UpdateModelMat();


	// Light and bounding box
	std::vector<glm::vec3> boxVertices =
	{
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};
	std::vector<glm::vec3> boxNormals =
	{
		glm::vec3(0, -1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, 1, 0)
	};
	std::vector<GLuint> boxIndices =
	{
		0, 1, 2, // Down
		0, 2, 3,
		0, 4, 7, // Front
		0, 7, 3,
		3, 7, 6, // Right
		3, 6, 2,
		2, 6, 5, // Back
		2, 5, 1,
		1, 5, 4, // Left
		1, 4, 0,
		4, 5, 6, // Top
		4, 6, 7
	};
	auto boxVertexes = std::vector<Vertex>();
	auto lightVertexes = std::vector<Vertex>();
	auto boxElements = std::vector<GLuint>();
	for (int i = 0; i < boxIndices.size(); i++)
	{
		boxVertexes.push_back(Vertex{ boxVertices[boxIndices[i]], boxNormals[i / 6] * -1.0f });
		lightVertexes.push_back(Vertex{ boxVertices[boxIndices[i]] });
		boxElements.push_back(i);
	}
	Mesh box(boxVertexes, boxElements);
	box.scale = 20.0f;
	box.UpdateModelMat();

	// Light setup
	Mesh light(lightVertexes, boxElements);
	light.scale = 0.1f;
	glm::vec3 pos(0.0, 1.0, 0.0);
	light.worldPos = ub.lightPos;
	light.UpdateModelMat();

	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));
	piece.scale = 0.1f;
	piece.UpdateModelMat();


	// Point setup
	std::vector<glm::vec3> points =
	{
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f)
	};

	std::vector<GLuint> indices =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0
	};

	Points pointRenderer(100);
	pointRenderer.PushBack(points);

	objects.emplace_back(std::make_unique<Mesh>(floor));
	objects.emplace_back(std::make_unique<Mesh>(box));
	objects.emplace_back(std::make_unique<Mesh>(light));
	objects.emplace_back(std::make_unique<ChessModel>(piece));
}

void SceneController::DrawScene(const unsigned frame)
{
	//Handles inputs to camera
	cam.Inputs(window);
	//Updates camera matrix
	cam.UpdateMatrix(45.0f, 0.1f, 100.0f);

	glm::vec3 light_pos = glm::vec3(0.0f, 3.0f + glm::sin(glm::radians(static_cast<float>(frame)/10.0f)) * 2, 0.0f);
	objects[2]->worldPos = light_pos;

	UBO.Bind();
	char tempBuffer[92];
	memcpy(tempBuffer, &cam.cameraMatrix, sizeof(glm::mat4));
	memcpy(tempBuffer + sizeof(glm::mat4), &cam.position, sizeof(glm::vec4));
	memcpy(tempBuffer + 80, &light_pos, sizeof(glm::vec4));
	UBO.EditBufferRange(tempBuffer, 0, sizeof(tempBuffer));
	UBO.Unbind();

	int count = 0;
	for (auto&& o : objects)
	{
		// Default, Flat, Basic
		// Floor, Box, Light, Piece
		o->UpdateModelMat();
		if (count == 0)
		{
			o->Draw(*shaders[0]);
		}
		else if (count == 1 || count == 3)
		{
			o->Draw(*shaders[1]);
		}
		else if (count == 2)
		{
			o->Draw(*shaders[2]);
		}
		count++;
	}


	// Renders above everything
	//glClear(GL_DEPTH_BUFFER_BIT);
	//pointRenderer.Draw(basicShader, camera);
}

void SceneController::Clean() const
{
	for (auto&& s : shaders)
	{
		s->Delete();
	}
}
