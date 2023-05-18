#include "RenderSystem.h"

extern ECSController ecsController;

void RenderSystem::PreUpdate()
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::Update()
{
	assert(mWindow && "Window not set.");

	auto tex = ecsController.GetComponentType<Components::TextureInfo>();

	for (const auto& entity : mEntities)
	{

		const auto& [primitive_type, VAO_ID, shader_ID, size, color] = ecsController.GetComponent<Components::RenderInfo>(entity);

		// Update transform
		auto transform = ecsController.GetComponent<Components::Transform>(entity);
		transform.CalculateModelMat();

		// Bind vertex array
		glBindVertexArray(VAO_ID);

		// Bind shader
		glUseProgram(shader_ID);

		auto entitySignature = ecsController.GetEntitySignature(entity);

		glUniformMatrix4fv(glGetUniformLocation(shader_ID, "model"), 1, GL_FALSE, value_ptr(transform.modelMat));
		glUniform3fv(glGetUniformLocation(shader_ID, "color"), 1, value_ptr(color));

		if (entitySignature.test(tex))
		{
			const auto& [diffuse_ID, specular_ID] = ecsController.GetComponent<Components::TextureInfo>(entity);
			
			// Textures
			// Set texture uniform value
			glUniform1i(glGetUniformLocation(shader_ID, "diffuse0"), 0);
			// Activate texture unit
			glActiveTexture(GL_TEXTURE0);
			// Bind texture to 
			glBindTexture(GL_TEXTURE_2D, diffuse_ID);

			glUniform1i(glGetUniformLocation(shader_ID, "specular0"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_ID);
		}

		// Draw VAO
		if (primitive_type == GL_POINTS)
			glDrawArrays(primitive_type, 0, size);
		else
		{
			/*if (primitive_type == GL_LINES)
				glClear(GL_DEPTH_BUFFER_BIT);*/
			glDrawElements(primitive_type, size, GL_UNSIGNED_INT, nullptr);
		}
			
	}
}

void RenderSystem::PostUpdate()
{
	glfwSwapBuffers(mWindow);
}

void RenderSystem::Clean()
{
	
}

void RenderSystem::SetWindow(GLFWwindow* window)
{
	mWindow = window;
}
