#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <random>

#include "core/GUI.h"
#include "core/UniformBufferManager.h"
#include "core/WindowManager.h"
#include "core/World.h"

#include "components/Components.h"

#include "renderer/RenderSystem.h"
#include "renderer/Texture.h"

#include "physics/PhysicsSystem.h"

#include "renderables/Lines.h"
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/ObjModel.h"
#include "renderables/Points.h"

#include "math/mesh/MeshSimplify.h"
#include "math/mesh/SimpleShapes.h"
#include "utils/Timer.h"
#include "utils/Logger.h"

// Force use of discrete Nvidia GPU
#ifdef _WIN32
#include <windows.h>
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // Optimus: force switch to discrete GPU
	// __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;//AMD
}

#endif

World world("log.txt", true);

int main()
{
	Utils::Timer timer("Setup");

	// Window creation
	// TODO: Add callbacks
	Core::WindowManager windowManager("OpenGL Window", 900, 900, true);
	GLFWwindow* window = windowManager.GetWindow();

	GUI GUI{ window };

	// Camera creation
	const auto& windowDimensions = windowManager.GetWindowDimensions();
	Camera cam { windowDimensions.first,windowDimensions.second, glm::vec3(0.0f, 1.0f, 7.0f) };
	// Used to update camera dimensions
	// TODO: Make window resizing and other initialization cleaner
	windowManager.SetCamera(&cam);

	// Register components
	world.RegisterComponent<Components::Transform>();
	world.RegisterComponent<Components::RenderInfo>();
	world.RegisterComponent<Components::DiffuseTextureInfo>();
	world.RegisterComponent<Components::SpecularTextureInfo>();
	world.RegisterComponent<Components::Rigidbody>();

	// Create RenderSystem and add dependencies
	auto renderSystem = world.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());

	// Create PhysicsSystem
	auto physicsSystem = world.RegisterSystem<PhysicsSystem>();
	auto& tree = physicsSystem->tree;

	// Set RenderSystem signature
	{
		Signature signature;
		signature.set(world.GetComponentType<Components::Transform>());
		signature.set(world.GetComponentType<Components::RenderInfo>());
		world.SetSystemSignature<RenderSystem>(signature);
	}

	// Set PhysicsSystem signature
	{
		Signature signature;
		signature.set(world.GetComponentType<Components::Transform>());
		signature.set(world.GetComponentType<Components::Rigidbody>());
		world.SetSystemSignature<PhysicsSystem>(signature);
	}

	Shader basicShader("basic.vert", "basic.frag");
	Shader flatShader("flat.vert", "flat.frag");
	Shader defaultShader("default.vert", "default.frag");
	Shader diffuseShader("diffuse.vert", "diffuse.frag");
	basicShader.mUniforms.reset(static_cast<size_t>(UniformBlockConfig::LIGHTING));

	// Wood floor setup
	const auto [planeVerts, planeInds] = Utils::PlaneData();
	Texture textures[]
	{
		Texture("planks.png", GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png",  GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Model floor(planeVerts, planeInds, tex[0]);
	floor.ShaderID = defaultShader.ID;
	floor.Scale(10.0f);
	floor.AddToECS();



	// Initialize random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-2, 2); // range for random positions

	// Number of cubes to generate
	// int numCubes = 100;
	//
	// for (int i = 0; i < numCubes; ++i) {
	// 	Mesh cube(cubeData);
	// 	cube.SetPosition(glm::vec3(dis(gen), abs(dis(gen)), dis(gen))); // random position
	// 	cube.Scale(0.1f);
	// 	cube.ShaderID = flatShader.ID;
	// 	cube.AddToECS();
	// 	physicsSystem->AddToTree(cube);
	// }

	const ModelData sphereData = Utils::UVSphereData(20,20, 1);
	Model light(sphereData);
	light.Scale(0.1f);
	light.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	light.ShaderID = basicShader.ID;
	light.SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	light.AddToECS();
	auto& lightPos = world.GetComponent<Components::Transform>(light.mEntityID).worldPos;


	// Mesh dragon("dragon.dat", false);
	// dragon.Scale(20.0f);
	// dragon.SetPosition(glm::vec3(1.0f, 0.5f, 0.0f));
	// dragon.ShaderID = flatShader.ID;
	// dragon.SetColor(glm::vec3(0.7f, 0.0f, 0.0f));
	// // dragon.transform.SetRotationEuler(glm::vec3(-90.0f, 0.0f, 0.0f));
	// dragon.AddToECS();

	// ObjModel sponza("sponza/", "sponza.obj");
	// sponza.Scale(0.5f);
	// sponza.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	// sponza.SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	// sponza.ShaderID(flatShader.ID);
	// sponza.AddToECS();

	// physicsSystem->AddToTree(light);
	// physicsSystem->AddToTree(dragon);

	// Box showing collision between objects
	// Lines collideBox(100000);
	// collideBox.mColor = glm::vec3(1.0f, 0.0f, 0.0f);
	// collideBox.ShaderID = basicShader.ID;
	// collideBox.AddToECS();
	//
	// // Constraint bounding box
	// // Lines boundsBox(10000);
	// // boundsBox.ShaderID = basicShader.ID;
	// // boundsBox.AddToECS();
	//
	// // Debug bounding boxes
	// Lines boxRenderer(20000);
	// boxRenderer.ShaderID = basicShader.ID;
	// boxRenderer.AddToECS();
	//
	// dragon.transform.CalculateModelMat();
	//
	// // Shows how complicated the mesh is
	// Lines rootRenderer(20589577);
	// rootRenderer.ShaderID = basicShader.ID;
	// rootRenderer.AddToECS();
	// rootRenderer.SetRenderingEnabled(false);
	//
	// Lines parentRenderer(41179129);
	// parentRenderer.ShaderID = basicShader.ID;
	// parentRenderer.AddToECS();
	// parentRenderer.SetRenderingEnabled(false);


	// bunny.transform.CalculateModelMat();
	// bunny.InitTree();
	// rootRenderer.PushBoundingBoxes(bunny.mTree.GetBoxes(bunny.transform.modelMat, true));
	// parentRenderer.PushBoundingBoxes(bunny.mTree.GetBoxes(bunny.transform.modelMat, false));


	// boundsBox.Clear();
	// boundsBox.PushBoundingBox(BoundingBox{ glm::vec3(-1.5f, 0.0f, -1.5f), glm::vec3(1.5f, 3.0f, 1.5f) });

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
	UBO.BindShader(diffuseShader);

	double lastFPSTime, currentTime;
	lastFPSTime = currentTime = glfwGetTime();

	unsigned int fpsFrameCount = 0;

	float time, mspf, fps;
	time = mspf = fps = 0.0f;

	std::cout << timer.ToString() << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		renderSystem->PreUpdate();

		float dt_mill = static_cast<float>(glfwGetTime() - currentTime) * 1000;

		// Move entities
		lightPos = glm::vec3(glm::sin(glm::radians(time / 20.0f))*3.0f, 3.0f, glm::cos(glm::radians(time / 20.0f))*3.0f);
		// lightPos = glm::vec3(glm::sin(glm::radians(time / 30.0f)) / 3.0f + 1.0f, 0.7f, 0.0f);
		// lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
		light.transform.worldPos = lightPos;

		// TODO: Create better position update system than reinserting into tree
		// tree.UpdateEntity(light.mEntityID, light.CalcBoundingBox());

		// physicsSystem->Update(dt_mill);

		// boxRenderer.Clear();
		// if (GUI.config.showDynamicBoxes)
		// {
		// 	boxRenderer.PushBoundingBoxes(tree.GetAllBoxes(GUI.config.showOnlyDynamicLeaf));
		// }
		//
		// collideBox.Clear();
		// const auto collidedEntities = tree.ComputeCollisionPairs();
		// for (const auto entity : collidedEntities)
		// {
		// 	collideBox.PushBoundingBox(tree.GetBoundingBox(entity));
		// }
		//
		// if (GUI.config.showStaticBoxes.changed || GUI.config.showOnlyStaticLeaf.changed)
		// {
		// 	rootRenderer.SetRenderingEnabled(GUI.config.showStaticBoxes.value && GUI.config.showOnlyStaticLeaf.value);
		// 	parentRenderer.SetRenderingEnabled(GUI.config.showStaticBoxes.value && !GUI.config.showOnlyStaticLeaf.value);
		// }
		//
		// if (GUI.config.regenStaticTree)
		// {
		// 	LOG(LOG_WARNING) << "Regenerating static tree.\n";
		// 	dragon.InitTree();
		//
		// 	auto rootBoxes = dragon.mTree.GetBoxes(dragon.transform.modelMat, true);
		// 	auto parentBoxes = dragon.mTree.GetBoxes(dragon.transform.modelMat, false);
		//
		// 	std::cout << "Root boxes: " << parentBoxes.size() << std::endl;
		//
		// 	rootRenderer.Clear();
		// 	rootRenderer.PushBoundingBoxes(rootBoxes);
		// 	parentRenderer.Clear();
		// 	parentRenderer.PushBoundingBoxes(parentBoxes);
		// }

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
			LOG_WRITE();
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
		UBO.UpdateData(cam, world.GetComponent<Components::Transform>(light.mEntityID).worldPos);

		std::string fpsString("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMSPF: " + std::to_string(mspf));

		renderSystem->Update();
		GUI.NewFrame();

		GUI.StartWindow("Performance");
		GUI.Text(fpsString.c_str());
		GUI.EndWindow();

		GUI.ShowConfigWindow();
		GUI.RenderLog(LOG_CONTENTS(), LOG_LINE_LEVELS());

		GUI.Render();

		renderSystem->PostUpdate();
	}

	GL_CHECK();

	world.Clean();
	GUI.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();


	return 0;
}
