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

Vertex boardVertexes[] =
{
	Vertex{glm::vec3(-1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0,0.0)},
	Vertex{glm::vec3(1.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0,0.0)},
	Vertex{glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(1.0,1.0)},
	Vertex{glm::vec3(-1.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0), glm::vec2(0.0,1.0)}
};

GLuint boardIndices[] =
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

	Shader chessShader("chess.vert", "chess.frag");
	ChessModel piece(ChessPiece(pawn), glm::vec3(0.0f, 0.0f, 0.0f));
	piece.scale = 0.01f;
	piece.updateModelMat();


	// Point setup
	Shader basicShader("basic.vert", "basic.frag");
	std::vector<glm::vec3> points = {glm::vec3(- 1.0, -1.0, -1.0)};

	Points pointRenderer(points);

	Shader defaultShader("default.vert", "default.frag");
	// Texture data
	Texture textures[]
	{
		Texture("planks.png", "diffuse", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png", "specular", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE)
	};
	std::vector <Vertex> verts(boardVertexes, boardVertexes + sizeof(boardVertexes) / sizeof(Vertex));
	std::vector <GLuint> ind(boardIndices, boardIndices + sizeof(boardIndices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh floor(verts, ind, tex);

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

	chessShader.Activate();
	glUniform4f(chessShader.GetUniformLocation("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(chessShader.GetUniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);

	defaultShader.Activate();
	glUniform4f(defaultShader.GetUniformLocation("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(defaultShader.GetUniformLocation("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	

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

		basicShader.Activate();
		light.worldPos = glm::vec3(0.0f, 1.0f, glm::sin(glm::radians((float)frame / 5)) / 3);
		light.updateModelMat();

		chessShader.Activate();
		glUniform3f(chessShader.GetUniformLocation("lightPos"), light.worldPos.x, light.worldPos.y, light.worldPos.z);

		defaultShader.Activate();
		glUniform3f(defaultShader.GetUniformLocation("lightPos"), light.worldPos.x, light.worldPos.y, light.worldPos.z);

		frame++;

		//Handles inputs to camera
		camera.Inputs(window);
		//Updates camera matrix
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);


		piece.Draw(chessShader, camera);
		light.Draw(basicShader, camera);
		floor.Draw(defaultShader, camera);

		
		// Renders above everything
		glClear(GL_DEPTH_BUFFER_BIT);
		pointRenderer.Draw(basicShader, camera);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Delete all objects that we created
	chessShader.Delete();
	basicShader.Delete();
	defaultShader.Delete();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}