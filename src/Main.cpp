#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#include "core/ECS.h"
#include "components/Components.h"
#include "core/UniformBufferManager.h"
#include "renderer/RenderSystem.h"
#include "renderables/ChessModel.h"
#include "core/WindowManager.h"
#include "renderables/Model.h"
#include "renderables/Mesh.h"
#include "renderer/Texture.h"

ECSController ecsController;

int main()
{
	// Window creation
	Core::WindowManager windowManager;
	windowManager.Init("OpenGL Window", 800, 800);
	GLFWwindow* window = windowManager.GetWindow();

	// Camera creation
	Camera cam{800,800, glm::vec3(0.0f, 1.0f, 7.0f)};

	// Initialize ECS
	ecsController.Init();

	// Register components
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();

	// Create RenderSystem and add dependencies
	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());
	renderSystem->SetCamera(&cam);

	// Set RenderSystem signature
	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	Shader flatShader("flat.vert", "flat.frag");
	Shader basicShader("basic.vert", "basic.frag");

	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));
	piece.ShaderID = flatShader.ID; // TODO: Make this easier to initialize
	piece.InitECS();

	Model bunny("bunny.dat", false);
	bunny.ShaderID = flatShader.ID;
	bunny.InitECS();
	bunny.GetTransform().worldPos = glm::vec3(1.0f, 0.0f, 0.0f);

	// Wood floor setup
	Vertex board_vertexes[] =
	{
		Vertex{glm::vec3(-1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
		Vertex{glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 3.0)},
		Vertex{glm::vec3(-1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 3.0)}
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
	floor.InitECS();
	

	// Manage Uniform Buffer
	Core::UniformBufferManager UBO;
	UBO.SetCamera(&cam);
	// Allocate buffer in OpenGL
	UBO.Allocate();
	// Bind uniform ranges in the buffer
	UBO.InitBind();

	// TODO: Make this applicable to all shaders
	// Maybe create some sort of bitset with dependencies for uniform blocks
	// Could have these dependencies in actual shader file, shader class reads these and updates accordingly
	UBO.BindShader(flatShader);

	while (!glfwWindowShouldClose(window))
	{
		windowManager.ProcessInputs();
		cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos());
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
		UBO.UpdateData();

		renderSystem->PreUpdate();
		renderSystem->Update();
		renderSystem->PostUpdate();
	}

	ecsController.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();

	return 0;
}
