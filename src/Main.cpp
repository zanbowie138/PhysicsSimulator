#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Camera.h"
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/ChessModel.h"
#include "renderables/Points.h"
#include "renderables/Lines.h"

constexpr unsigned int screen_width = 800;
constexpr unsigned int screen_height = 800;

constexpr unsigned int aliasing_samples = 8;

Vertex light_vertices[] =
{
	//     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f, 0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, 0.1f)},
	Vertex{glm::vec3(-0.1f, 0.1f, 0.1f)},
	Vertex{glm::vec3(-0.1f, 0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, 0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, 0.1f, 0.1f)}
};

GLuint light_indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

Vertex board_vertexes[] =
{
	Vertex{glm::vec3(-1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0, 0.0)},
	Vertex{glm::vec3(1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0, 0.0)},
	Vertex{glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0, 1.0)},
	Vertex{glm::vec3(-1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0, 1.0)}
};

GLuint board_indices[] =
{
	0, 1, 2,
	2, 3, 0
};

int main()
{
	// Initialize GLFW
	glfwInit();

	// Set GLFW version to 3.3 and use core profile (uses modern OpenGL functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, aliasing_samples);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates window, if window is null, throw error
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "OpenGLWindow", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

	// Creates object that stores the state of this instance of OpenGL (i think)
	glfwMakeContextCurrent(window);

	gladLoadGL();

	//Specifies the transformation from normalized coordinates (0-1) to screen coordinates
	glViewport(0, 0, screen_width, screen_height);

	Shader chess_shader("chess.vert", "chess.frag");
	ChessModel piece(static_cast<ChessPiece>(pawn), glm::vec3(0.0f, 0.0f, 0.0f));
	piece.scale = 0.01f;
	piece.UpdateModelMat();

	std::vector<glm::vec3> box_vertices =
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

	std::vector<glm::vec3> box_normals =
	{
		glm::vec3(0, -1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, 1, 0)
	};

	std::vector<GLuint> box_indices =
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

	auto box_vertexes = std::vector<Vertex>();
	auto box_elements = std::vector<GLuint>();
	for (int i = 0; i < box_indices.size(); i++)
	{
		box_vertexes.push_back(Vertex{box_vertices[box_indices[i]], box_normals[i / 6]});
		box_elements.push_back(i);
	}
	Mesh box(box_vertexes, box_elements);


	// Point setup
	Shader basicShader("basic.vert", "basic.frag");
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

	// Wood floor setup
	Shader defaultShader("default.vert", "default.frag");
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
	floor.scale = 1;
	floor.UpdateModelMat();

	// Light setup
	std::vector<Vertex> lightVerts(light_vertices, light_vertices + sizeof(light_vertices) / sizeof(Vertex));
	std::vector<GLuint> lightInd(light_indices, light_indices + sizeof(light_indices) / sizeof(GLuint));
	Mesh light(lightVerts, lightInd);


	// Set light color
	glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Creates the light model matrix
	auto lightPos = glm::vec3(0.5f, 3.0f, 0.5f);

	chess_shader.Activate();
	glUniform4f(chess_shader.GetUniformLocation("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(chess_shader.GetUniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);

	defaultShader.Activate();
	glUniform4f(defaultShader.GetUniformLocation("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(defaultShader.GetUniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);


	glEnable(GL_DEPTH_TEST);

	// Enable anti-aliasing
	glEnable(GL_MULTISAMPLE);

	Camera camera(screen_width, screen_height, glm::vec3(0.0f, 1.0f, 5.0f));

	unsigned int frame = 0;
	double lastTime = glfwGetTime();
	double currentTime = 0.0;
	int nbFrames = 0;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{
			// If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f mspf, %f fps\n", 1000.0 / static_cast<double>(nbFrames), static_cast<double>(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}


		basicShader.Activate();
		light.worldPos = glm::vec3(glm::cos(glm::radians(static_cast<float>(frame) / 15)) / 1.3, 1.0f,
		                           glm::sin(glm::radians(static_cast<float>(frame) / 15)) / 1.3);
		light.UpdateModelMat();

		chess_shader.Activate();
		glUniform3f(chess_shader.GetUniformLocation("lightPos"), light.worldPos.x, light.worldPos.y, light.worldPos.z);

		defaultShader.Activate();
		glUniform3f(defaultShader.GetUniformLocation("lightPos"), light.worldPos.x, light.worldPos.y, light.worldPos.z);

		frame++;

		//Handles inputs to camera
		camera.Inputs(window);
		//Updates camera matrix
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);


		piece.Draw(chess_shader, camera);
		light.Draw(basicShader, camera);
		floor.Draw(defaultShader, camera);
		box.Draw(chess_shader, camera);


		// Renders above everything
		glClear(GL_DEPTH_BUFFER_BIT);
		pointRenderer.Draw(basicShader, camera);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Delete all objects that we created
	chess_shader.Delete();
	basicShader.Delete();
	defaultShader.Delete();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}
