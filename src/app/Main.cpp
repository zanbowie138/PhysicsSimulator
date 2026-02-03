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

#include "scene/SceneImporter.h"

#include "utils/Timer.h"
#include "utils/Logger.h"
#include "utils/Raycast.h"
#include "utils/Exceptions.h"

// Force use of discrete Nvidia GPU
#ifdef _WIN32
#include <windows.h>
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // Optimus: force switch to discrete GPU
	// __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;//AMD
}
#undef ERROR // Prevent Windows ERROR macro from conflicting with Logger::ERROR
#endif

World world("log.txt", true);

int main()
{
	try {
		Utils::Timer timer("Setup");

		// Window creation
		Core::WindowManager windowManager("OpenGL Window", 900, 900, true);
		GLFWwindow* window = windowManager.GetWindow();

		GUI GUI{ window };

		// Camera creation
		const auto& windowDimensions = windowManager.GetWindowDimensions();
		Camera cam { windowDimensions.first,windowDimensions.second, glm::vec3(0.0f, 1.0f, 7.0f) };
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

		auto basicShader = Shader::Create("basic.vert", "basic.frag");
		if (!basicShader) {
			LOG(LOG_ERROR) << "Failed to load basic shader\n";
			return 1;
		}
		auto flatShader = Shader::Create("flat.vert", "flat.frag");
		if (!flatShader) {
			LOG(LOG_ERROR) << "Failed to load flat shader\n";
			return 1;
		}
		auto defaultShader = Shader::Create("default.vert", "default.frag");
		if (!defaultShader) {
			LOG(LOG_ERROR) << "Failed to load default shader\n";
			return 1;
		}
		auto diffuseShader = Shader::Create("diffuse.vert", "diffuse.frag");
		if (!diffuseShader) {
			LOG(LOG_ERROR) << "Failed to load diffuse shader\n";
			return 1;
		}
		basicShader->mUniforms.reset(static_cast<size_t>(UniformBlockConfig::LIGHTING));

		// Create shader map for lua scene loading
		std::unordered_map<std::string, GLuint> shaders;
		shaders["basic"] = basicShader->ID;
		shaders["flat"] = flatShader->ID;
		shaders["default"] = defaultShader->ID;
		shaders["diffuse"] = diffuseShader->ID;

		// Initialize scene from lua
		if (!InitializeSceneFromLua(world, "test.lua", shaders)) {
			LOG(LOG_ERROR) << "Failed to initialize scene\n";
			return 1;
		}

	// Scene now loaded from lua - hardcoded setup commented out
	// const auto [planeVerts, planeInds] = Utils::PlaneData();
	// Texture textures[]
	// {
	// 	Texture("planks.png", GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE),
	// 	Texture("planksSpec.png",  GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE)
	// };
	// std::vector tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Model floor(planeVerts, planeInds, tex[0]);
	// floor.ShaderID = defaultShader.ID;
	// floor.Scale(10.0f);
	// floor.AddToECS();
	//
	//
	//
	// // Initialize random number generator
	// std::random_device rd;
	// std::mt19937 gen(rd());
	// std::uniform_real_distribution<> dis(-2, 2); // range for random positions
	//
	// const auto cubeData = Utils::CubeData();
	//
	// // Number of cubes to generate
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
	//
	// const ModelData sphereData = Utils::UVSphereData(20,20, 1);
	// Model light(sphereData);
	// light.Scale(0.1f);
	// light.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	// light.ShaderID = basicShader.ID;
	// light.SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	// light.AddToECS();

		// Get light entity ID from lua scene
		// TODO: Return entity IDs by name from scene loader
		Entity lightEntity = 2;
		glm::vec3* lightPos = nullptr;
		try {
			lightPos = &world.GetComponent<Components::Transform>(lightEntity).worldPos;
		} catch (const ECSException& e) {
			LOG(LOG_WARNING) << "Light entity not found or missing Transform, using default position\n";
			lightEntity = 0; // Invalid entity
		}

		// Box showing collision between objects
		// Lines hitBox(100);
		// hitBox.mColor = glm::vec3(0.0f, 1.0f, 0.0f);
		// hitBox.ShaderID = basicShader->ID;
		// hitBox.AddToECS();
		//
		// // Box showing collision between objects
		// Lines collideBox(10000);
		// collideBox.mColor = glm::vec3(1.0f, 0.0f, 0.0f);
		// collideBox.ShaderID = basicShader->ID;
		// collideBox.AddToECS();
		//
		// // Debug bounding boxes
		// Lines boxRenderer(20000);
		// boxRenderer.ShaderID = basicShader->ID;
		// boxRenderer.AddToECS();
		//
		// Lines debugLineRenderer(10000);
		// debugLineRenderer.ShaderID = basicShader->ID;
		// debugLineRenderer.AddToECS();



	// bunny.transform.CalculateModelMat();
	// bunny.InitTree();
	// rootRenderer.PushBoundingBoxes(bunny.mTree.GetBoxes(bunny.transform.modelMat, true));
	// parentRenderer.PushBoundingBoxes(bunny.mTree.GetBoxes(bunny.transform.modelMat, false));


	// boundsBox.Clear();
	// boundsBox.PushBoundingBox(BoundingBox{ glm::vec3(-1.5f, 0.0f, -1.5f), glm::vec3(1.5f, 3.0f, 1.5f) });

		// Manage Uniform Buffer
		Core::UniformBufferManager UBO;
		UBO.AllocateBuffer();
		UBO.DefineRanges();

		// Set uniform blocks in shaders to UBO indexes
		UBO.BindShader(*basicShader);
		UBO.BindShader(*defaultShader);
		UBO.BindShader(*flatShader);
		UBO.BindShader(*diffuseShader);

		double lastFPSTime, currentTime;
		lastFPSTime = currentTime = glfwGetTime();

		unsigned int fpsFrameCount = 0;

		float time, mspf, fps;
		time = mspf = fps = 0.0f;

		std::cout << timer.ToString() << std::endl;

		Entity entity = Entity();
		bool entitySelected = false;

		while (!glfwWindowShouldClose(window))
		{
		renderSystem->PreUpdate();

		float dt_mill = static_cast<float>(glfwGetTime() - currentTime) * 1000;

		// Move entities
		// lightPos = glm::vec3(glm::sin(glm::radians(time / 20.0f))*3.0f, 3.0f, glm::cos(glm::radians(time / 20.0f))*3.0f);
		// lightPos = glm::vec3(glm::sin(glm::radians(time / 30.0f)) / 3.0f + 1.0f, 0.7f, 0.0f);
		// lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
		// Update light position via ECS component

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
		UBO.UpdateData(cam, world.GetComponent<Components::Transform>(lightEntity).worldPos);

		// if (windowManager.TestInput(InputButtons::LEFT_MOUSE))
		// {
		// 	LOG(LOG_INFO) << "Mouse position: " << glm::to_string(windowManager.GetMousePosNormalized()) << "\n";
		// 	Ray r = Utils::ScreenPointToRay(windowManager.GetMousePosNormalized(), cam.cameraMatrix);
		// 	debugLineRenderer.Clear();
		// 	debugLineRenderer.PushRay(r, 10);
		// 	const auto [boxes, hit] = tree.QueryRayCollisions(r);
		// 	const auto [entityHit, hitEntity] = tree.QueryRay(r);
		// 	collideBox.Clear();
		// 	hitBox.Clear();
		// 	collideBox.PushBoundingBoxes(boxes);
		// 	entity = entityHit;
		// 	entitySelected = hitEntity;
		// 	if (hit)
		// 	{
		// 		LOG(LOG_INFO) << "Hit entity\n";
		// 		hitBox.PushBoundingBox(tree.GetBoundingBox(entityHit));
		// 	} else
		// 	{
		// 		LOG(LOG_INFO) << "No hit\n";
		// 	}
		// }
		std::string fpsString("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMSPF: " + std::to_string(mspf));

		renderSystem->Update();
		GUI.NewFrame();

		GUI.StartWindow("Performance");
		GUI.Text(fpsString.c_str());
		GUI.EndWindow();

		GUI.ShowConfigWindow();
		GUI.EntityInfo(entity, entitySelected);
		GUI.RenderLog(LOG_CONTENTS(), LOG_LINE_LEVELS());

		GUI.Render();

		renderSystem->PostUpdate();
	}

	GL_CHECK();

		world.Clean();
		GUI.Clean();

		windowManager.Shutdown();

		return 0;

	} catch (const EngineException& e) {
		LOG(LOG_ERROR) << "Fatal engine error: " << e.what() << "\n";
		return 1;
	} catch (const std::exception& e) {
		LOG(LOG_ERROR) << "Unexpected error: " << e.what() << "\n";
		return 1;
	}
}
