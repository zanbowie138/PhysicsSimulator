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

#include "renderables/Lines.h"
#include "renderables/Model.h"
#include "renderables/Mesh.h"

#include "utils/SimpleShapes.h"

ECSController ecsController;

int main()
{
	// TODO: Combine VBO, EBO into one class
	// TODO: Remove hardcoding ranges from UBO
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

	// Create PhysicsSystem
	auto physicsSystem = ecsController.RegisterSystem<PhysicsSystem>();

	// Set RenderSystem signature
	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	Shader basicShader("basic.vert", "basic.frag");
	Shader flatShader("flat.vert", "flat.frag");
	Shader defaultShader("default.vert", "default.frag");
	basicShader.mUniforms.reset(static_cast<size_t>(UniformBlockConfig::LIGHTING));

	Model piece("chess_models/king.bin", false);
	piece.ShaderID = flatShader.ID; // TODO: Make this easier to initialize
	piece.transform.scale = glm::vec3(0.01f);
	piece.mColor = glm::vec3(1.0f, 0.0f, 0.5f);
	piece.InitECS();
	physicsSystem->tree.InsertEntity(piece.mEntityID, piece.CalcBoundingBox());

	Model bunny("bunny.stl", true);
	bunny.ShaderID = flatShader.ID;
	bunny.transform.scale = glm::vec3(0.01f);
	bunny.transform.rotation = glm::vec3(-90, 0, 0);
	bunny.transform.worldPos = glm::vec3(1.0f, 0.0f, 0.0f);
	bunny.InitECS();
	physicsSystem->tree.InsertEntity(bunny.mEntityID, bunny.CalcBoundingBox());

	// Wood floor setup
	const auto [planeVerts, planeInds] = Utils::PlaneData();
	Texture textures[]
	{
		Texture("planks.png", GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png",  GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh floor(planeVerts, planeInds, tex);
	floor.ShaderID = defaultShader.ID;
	floor.transform.scale = glm::vec3(10.0f);
	floor.InitECS();

	const auto [cubeVerts, cubeInds] = Utils::CubeData(true);
	Mesh light(cubeVerts, cubeInds);
	light.transform.worldPos = glm::vec3(0.0f, 1.0f, 0.0f);
	light.transform.scale = glm::vec3(0.1f);
	light.ShaderID = basicShader.ID;
	light.InitECS();
	auto& lightPos = ecsController.GetComponent<Components::Transform>(light.mEntityID).worldPos;


	Mesh cube(cubeVerts, cubeInds);
	cube.transform.worldPos = glm::vec3(-1.0f, 1.0f, static_cast<float>(1));
	cube.transform.scale = glm::vec3(0.3f);
	cube.ShaderID = flatShader.ID;
	cube.InitECS();
	physicsSystem->tree.InsertEntity(cube.mEntityID, cube.CalcBoundingBox());
	auto& cubePos = ecsController.GetComponent<Components::Transform>(cube.mEntityID);

	Lines boxRenderer(1000);
	boxRenderer.ShaderID = basicShader.ID;
	boxRenderer.InitECS();

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

	double lastTime = glfwGetTime();
	double currentTime = 0.0;
	unsigned int fpsFrameCount = 0;
	float mspf = 0.0f;
	float fps = 0.0f;
	float time = 0;
	unsigned int frame = 0;

	while (!glfwWindowShouldClose(window))
	{
		renderSystem->PreUpdate();
		GUI.NewFrame();

		lightPos = glm::vec3(glm::sin(glm::radians(time / 10.0f))/2.0f, 1.0f, glm::cos(glm::radians(time / 10.0f))/2.0f);

		cubePos.worldPos = cam.position + glm::vec3(0.0f, 0.0f, -2.0f);
		cube.transform.worldPos = cam.position + glm::vec3(0.0f, 0.0f, -2.0f);
		physicsSystem->tree.RemoveEntity(cube.mEntityID);
		physicsSystem->tree.InsertEntity(cube.mEntityID, cube.CalcBoundingBox());
		/*physicsSystem->tree.ComputePairs();
		if (physicsSystem->tree.mCollisions.empty())
		{
			std::cout << physicsSystem->tree.mCollisions.size() << std::endl;
		}*/

		const auto boxes = physicsSystem->tree.GetAllBoxes(false);
		boxRenderer.Clear();
		for (const auto& box : boxes)
		{

			boxRenderer.PushBack(box);
		}


		// Update window input bitset
		windowManager.ProcessInputs(!GUI.MouseOver());
		GUI.SetMouse(windowManager.mouseShown);
		// Move camera based on window inputs
		cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos());
		// Update camera matrix
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
		// Update uniform buffer
		UBO.UpdateData(cam, ecsController.GetComponent<Components::Transform>(light.mEntityID).worldPos);

		time += static_cast<float>((glfwGetTime() - currentTime) * 1000.0f);
		currentTime = glfwGetTime();
		fpsFrameCount++;
		if (currentTime - lastTime >= 1.0)
		{
			// If last fps update() was more than 1 sec ago
			mspf = 1000.0f / static_cast<float>(fpsFrameCount);
			fps = static_cast<float>(fpsFrameCount);
			fpsFrameCount = 0;
			lastTime += 1.0;
		}
		frame++;

		std::stringstream ss;
		ss << "FPS: " << fps << "\nMSPF: " << mspf;
		std::string s = ss.str();

		renderSystem->Update();
		GUI.Draw(s.c_str());
		GUI.Render();
		renderSystem->PostUpdate();
	}

	ecsController.Clean();
	GUI.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();


	return 0;
}
