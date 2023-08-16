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
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/Points.h"

#include "utils/SimpleShapes.h"
#include "utils/MeshSimplify.h"
#include "utils/Timer.h"

ECSController ecsController;

int main()
{
	// TODO: Combine VBO, EBO into one class

	Utils::Timer timer("Setup");

	// Window creation
	// TODO: Add callbacks
	Core::WindowManager windowManager;
	windowManager.Init("OpenGL Window", 900, 900);
	GLFWwindow* window = windowManager.GetWindow();

	GUI GUI;
	GUI.Init(window);

	// Camera creation
	const auto& windowDimensions = windowManager.GetWindowDimensions();
	Camera cam{ windowDimensions.first,windowDimensions.second, glm::vec3(0.0f, 1.0f, 7.0f) };
	// Used to update camera dimensions
	// TODO: Make window resizing and other initialization cleaner
	windowManager.SetCameraPointer(&cam);

	// Initialize ECS
	ecsController.Init();

	// Register components
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();
	ecsController.RegisterComponent<Components::TextureInfo>();
	ecsController.RegisterComponent<Components::Rigidbody>();

	// Create RenderSystem and add dependencies
	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());

	// Create PhysicsSystem
	auto physicsSystem = ecsController.RegisterSystem<PhysicsSystem>();
	auto& tree = physicsSystem->tree;

	// Set RenderSystem signature
	{
		Signature signature;
		signature.set(ecsController.GetComponentType<Components::Transform>());
		signature.set(ecsController.GetComponentType<Components::RenderInfo>());
		ecsController.SetSystemSignature<RenderSystem>(signature);
	}

	// Set PhysicsSystem signature
	{
		Signature signature;
		signature.set(ecsController.GetComponentType<Components::Transform>());
		signature.set(ecsController.GetComponentType<Components::Rigidbody>());
		ecsController.SetSystemSignature<PhysicsSystem>(signature);
	}

	Shader basicShader("basic.vert", "basic.frag");
	Shader flatShader("flat.vert", "flat.frag");
	Shader defaultShader("default.vert", "default.frag");
	basicShader.mUniforms.reset(static_cast<size_t>(UniformBlockConfig::LIGHTING));

	// Wood floor setup
	const auto [planeVerts, planeInds] = Utils::PlaneData();
	Texture textures[]
	{
		Texture("planks.png", GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png",  GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Model floor(planeVerts, planeInds, tex);
	floor.ShaderID = defaultShader.ID;
	floor.transform.scale = glm::vec3(10.0f);
	floor.InitECS();

	const auto cubeData = Utils::CubeData(true);
	Mesh light(cubeData.vertices, cubeData.indices);
	light.transform.worldPos = glm::vec3(0.0f, 1.0f, 0.0f);
	light.transform.scale = glm::vec3(0.07f);
	light.ShaderID = basicShader.ID;
	light.InitECS();
	auto& lightPos = ecsController.GetComponent<Components::Transform>(light.mEntityID).worldPos;


	Mesh cube(cubeData.vertices, cubeData.indices);
	cube.transform.worldPos = glm::vec3(-1.0f, 1.0f, static_cast<float>(1));
	cube.transform.scale = glm::vec3(0.3f);
	cube.ShaderID = flatShader.ID;
	cube.InitECS();

	const ModelData sphereData = Utils::UVSphereData(20,20);
	Model sphere(sphereData);
	sphere.transform.scale = glm::vec3(0.5f);
	sphere.transform.worldPos = glm::vec3(0.0f, 1.0f, 0.0f);
	sphere.ShaderID = flatShader.ID;
	sphere.mColor = glm::vec3(0.5f, 0.3f, 1.0f);
	sphere.InitECS();

	Mesh bunny("bunny.stl", true);
	bunny.transform.scale = glm::vec3(0.01f);
	bunny.transform.worldPos = glm::vec3(1.0f, 0.5f, 0.0f);
	bunny.ShaderID = flatShader.ID;
	bunny.transform.SetRotationEuler(glm::vec3(-90.0f, 0.0f, 0.0f));
	bunny.InitECS();

	physicsSystem->AddToTree(light);
	physicsSystem->AddToTree(cube);
	//physicsSystem->AddRigidbody(sphere);
	physicsSystem->AddToTree(sphere);

	// Box showing collision between objects
	Lines collideBox(1000);
	collideBox.mColor = glm::vec3(1.0f, 0.0f, 0.0f);
	collideBox.ShaderID = basicShader.ID;
	collideBox.InitECS();

	// Constraint bounding box
	Lines boundsBox(10000);
	boundsBox.ShaderID = basicShader.ID;
	boundsBox.InitECS();

	// Debug bounding boxes
	Lines boxRenderer(20000);
	boxRenderer.ShaderID = basicShader.ID;
	boxRenderer.InitECS();

	// Shows how complicated the mesh is
	Lines meshRenderer(1000000);
	meshRenderer.ShaderID = basicShader.ID;
	meshRenderer.InitECS();

	bunny.transform.CalculateModelMat();
	meshRenderer.PushBack(MeshData{ bunny.vertices, bunny.indices }, bunny.transform.modelMat);

	Utils::Timer tmd("Mesh Decimation");
	Utils::DecimateMesh(MeshData{ bunny.vertices, bunny.indices });
	std::cout << tmd.ToString() << std::endl;


	boundsBox.Clear();
	boundsBox.PushBack(BoundingBox{ glm::vec3(-1.5f, 0.0f, -1.5f), glm::vec3(1.5f, 3.0f, 1.5f) });

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

	double lastFPSTime, currentTime;
	lastFPSTime = currentTime = glfwGetTime();

	unsigned int fpsFrameCount = 0;

	float time, mspf, fps;
	time = mspf = fps = 0.0f;

	std::cout << timer.ToString() << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		renderSystem->PreUpdate();

		float dt_s = static_cast<float>(glfwGetTime() - currentTime);
		float dt_mill = dt_s * 1000;

		// Move entities
		lightPos = glm::vec3(glm::sin(glm::radians(time / 20.0f))/0.7f, 2.0f, glm::cos(glm::radians(time / 20.0f))/0.7f);
		//lightPos = glm::vec3(glm::sin(glm::radians(time / 30.0f)) / 3.0f + 1.0f, 0.7f, 0.0f);
		light.transform.worldPos = lightPos;
		tree.UpdateEntity(light.mEntityID, light.CalcBoundingBox());

		physicsSystem->Update(dt_s);

		boxRenderer.Clear();
		if (GUI.mConfigInfo.debugBoundingBoxes)
		{
			boxRenderer.PushBack(tree.GetAllBoxes(GUI.mConfigInfo.showOnlyLeafNodes));
		}
		

		collideBox.Clear();
		const auto collidedEntities = tree.ComputeCollisionPairs();
		for (const auto entity : collidedEntities)
		{
			collideBox.PushBack(tree.GetBoundingBox(entity));
		}

		currentTime = glfwGetTime();
		fpsFrameCount++;

		// Updates fps every second
		if (currentTime - lastFPSTime >= 1.0)
		{
			// If last fps update() was more than 1 sec ago
			mspf = 1000.0f / static_cast<float>(fpsFrameCount);
			fps = static_cast<float>(fpsFrameCount);
			fpsFrameCount = 0;
			lastFPSTime += 1.0;
		}

		time += dt_mill;

		// Update window input bitset
		windowManager.ProcessInputs(!GUI.MouseOver());
		GUI.SetMouse(windowManager.mouseShown);
		// Move camera based on window inputs
		cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos(), dt_mill);
		// Update camera matrix
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
		// Update uniform buffer
		UBO.UpdateData(cam, ecsController.GetComponent<Components::Transform>(light.mEntityID).worldPos);

		std::string fpsString("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMSPF: " + std::to_string(mspf));

		renderSystem->Update();
		GUI.NewFrame();

		GUI.StartWindow("Performance");
		GUI.Text(fpsString.c_str());
		GUI.EndWindow();

		GUI.ShowConfigWindow();

		GUI.Render();
		renderSystem->PostUpdate();
	}

	ecsController.Clean();
	GUI.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();


	return 0;
}
