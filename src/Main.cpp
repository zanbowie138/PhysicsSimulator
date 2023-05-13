#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#include "core/ECS.h"
#include "core/UniformBufferManager.h"
#include "core/WindowManager.h"
#include "core/GUI.h"

#include "components/Components.h"

#include "renderer/RenderSystem.h"
#include "renderer/Texture.h"

#include "physics/PhysicsSystem.h"

#include "renderables/ChessModel.h"
#include "renderables/Lines.h"
#include "renderables/Model.h"
#include "renderables/Mesh.h"

ECSController ecsController;

int main()
{
	// TODO: Combine VBO, EBO into one class
	// TODO: Remove hardcoding ranges from UBO
	// TODO: lines & points
	// Window creation
	Core::WindowManager windowManager;
	windowManager.Init("OpenGL Window", 800, 800);
	GLFWwindow* window = windowManager.GetWindow();

	GUI GUI;
	GUI.Init(window);

	// Camera creation
	Camera cam{800,800, glm::vec3(0.0f, 1.0f, 7.0f)};

	// Initialize ECS
	ecsController.Init();

	// Register components
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();
	ecsController.RegisterComponent<Components::TextureInfo>();

	// Create RenderSystem and add dependencies
	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());
	renderSystem->SetCamera(&cam);
	renderSystem->InitOpenGL();

	// Create PhysicsSystem
	auto physicsSystem = ecsController.RegisterSystem<PhysicsSystem>();

	// Set RenderSystem signature
	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	Shader flatShader("flat.vert", "flat.frag");
	Shader defaultShader("default.vert", "default.frag");
	Shader basicShader("basic.vert", "basic.frag");
	basicShader.mUniforms.reset(static_cast<size_t>(UniformBlockConfig::LIGHTING));

	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));
	piece.ShaderID = flatShader.ID; // TODO: Make this easier to initialize
	piece.transform.scale = glm::vec3(0.01f);
	piece.InitECS();

	Model bunny("bunny.dat", false);
	bunny.ShaderID = flatShader.ID;
	bunny.transform.scale = glm::vec3(0.01f);
	bunny.transform.rotation = glm::vec3(-90, 0, 0);
	bunny.transform.worldPos = glm::vec3(1.0f, 0.0f, 0.0f);
	bunny.InitECS();
	physicsSystem->tree.InsertEntity(bunny.mEntityID, bunny.CalcBoundingBox());

	// Wood floor setup
	Vertex board_vertexes[] =
	{
		Vertex{glm::vec3(-1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)},
		Vertex{glm::vec3(-1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)}
	};
	GLuint board_indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	Texture textures[]
	{
		Texture("planks.png", GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png",  GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector<Vertex> verts(board_vertexes, board_vertexes + sizeof(board_vertexes) / sizeof(Vertex));
	std::vector<GLuint> ind(board_indices, board_indices + sizeof(board_indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh floor(verts, ind, tex);
	floor.ShaderID = defaultShader.ID;
	floor.transform.scale = glm::vec3(10.0f);
	floor.InitECS();

	Lines boxRenderer(100);
	boxRenderer.ShaderID = flatShader.ID;
	boxRenderer.InitECS();
	boxRenderer.PushBack(physicsSystem->tree.GetBoundingBox(bunny.mEntityID));

	// Manage Uniform Buffer
	Core::UniformBufferManager UBO; 
	// Allocate buffer in OpenGL
	UBO.AllocateBuffer();
	// Bind uniform ranges in the buffer
	UBO.DefineRanges();

	// Set uniform blocks in shaders to UBO indexes
	UBO.BindShader(basicShader);
	UBO.BindShader(defaultShader);
	UBO.BindShader(flatShader);

	while (!glfwWindowShouldClose(window))
	{
		renderSystem->PreUpdate();
		GUI.NewFrame();

		// Update window input bitset
		windowManager.ProcessInputs(!GUI.MouseOver());
		GUI.SetMouse(windowManager.mouseShown);
		// Move camera based on window inputs
		cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos());
		// Update camera matrix
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
		// Update uniform buffer
		UBO.UpdateData(cam);

		renderSystem->Update();
		GUI.Draw();

		GUI.Render();
		renderSystem->PostUpdate();
	}

	ecsController.Clean();
	GUI.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();


	return 0;
}
