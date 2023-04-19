#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/ChessModel.h"
#include "renderables/Points.h"

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 800;

const unsigned int aliasingSamples = 8;

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
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

Vertex boardVertices[] =
{
	Vertex{}
};

int main()
{
	// Initialize GLFW
	glfwInit();

	// Set GLFW version to 3.3 and use core profile (uses modern OpenGL functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, aliasingSamples);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates window, if window is null, throw error
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGLWindow", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

	// Creates object that stores the state of this instance of OpenGL (i think)
	glfwMakeContextCurrent(window);

	gladLoadGL();

	//Specifies the transformation from normalized coordinates (0-1) to screen coordinates
	glViewport(0, 0, screenWidth, screenHeight);
	
	/*
	// Texture data
	Texture textures[]
	{
		Texture("planks.png", "diffuse", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png", "specular", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE)
	};*/

	Shader shaderProgram("chess.vert", "chess.frag");
	ChessModel piece(ChessPiece(pawn), glm::vec3(0.0f, 0.0f, 0.0f));
	piece.scale = 0.01;
	piece.updateModelMat();


	// Point setup
	Shader basicShader("basic.vert", "basic.frag");
	std::vector<glm::vec3> points = {glm::vec3(- 1.0, -1.0, -1.0)};

	//Points pointRenderer;

	/*
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh floor(verts, ind, tex);*/

	// Shader for light cube
	Shader lightShader("basic.vert", "basic.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Crate light mesh
	Mesh light(lightVerts, lightInd);


	// Set light color
	glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Creates the light model matrix
	glm::vec3 lightPos = glm::vec3(0.5f, 3.0f, 0.5f);
	glm::mat4 lightModel = glm::mat4(0.1f);

	shaderProgram.Activate();
	glUniform4f(shaderProgram.GetUniformLocation("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(shaderProgram.GetUniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	

	glEnable(GL_DEPTH_TEST);

	// Enable anti-aliasing
	glEnable(GL_MULTISAMPLE);

	Camera camera(screenWidth, screenHeight, glm::vec3(0.0f, 1.0f, 5.0f));

	unsigned int frame = 0;
	double lastTime = glfwGetTime();
	double currentTime = 0.0f;
	int nbFrames = 0;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f mspf, %f fps\n", 1000.0 / double(nbFrames), double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		lightShader.Activate();
		light.worldPos = glm::vec3(0.5f, glm::sin(glm::radians((float)frame/25)) * 3, 1.5f);
		light.updateModelMat();

		shaderProgram.Activate();
		glUniform3f(shaderProgram.GetUniformLocation("lightPos"), light.worldPos.x, light.worldPos.y, light.worldPos.z);

		frame++;

		//Handles inputs to camera
		camera.Inputs(window);
		//Updates camera matrix
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);


		piece.Draw(shaderProgram, camera);
		light.Draw(lightShader, camera);
		//pointRenderer.Draw(basicShader, camera);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Delete all objects that we created
	shaderProgram.Delete();
	lightShader.Delete();
	basicShader.Delete();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}