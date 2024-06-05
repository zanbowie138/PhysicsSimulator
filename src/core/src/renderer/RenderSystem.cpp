#include "RenderSystem.h"

extern World world;

void RenderSystem::PreUpdate() const
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::Update() const
{
	assert(mWindow && "Window not set.");

	auto tex = world.GetComponentType<Components::TextureInfo>();

	for (const auto& entity : mEntities)
	{
		const auto& renderInfo = world.GetComponent<Components::RenderInfo>(entity);

		if (!renderInfo.enabled) { continue; }

		// Update transform
		auto& transform = world.GetComponent<Components::Transform>(entity);
		transform.CalculateModelMat();

		// Bind vertex array
		glBindVertexArray(renderInfo.VAO_ID);

		// Bind shader
		glUseProgram(renderInfo.shader_ID);

		auto entitySignature = world.GetEntitySignature(entity);

		glUniformMatrix4fv(glGetUniformLocation(renderInfo.shader_ID, "model"), 1, GL_FALSE, glm::value_ptr(transform.modelMat));
		glUniform3fv(glGetUniformLocation(renderInfo.shader_ID, "color"), 1, glm::value_ptr(renderInfo.color));

		// Test if entity has a texture
		if (entitySignature.test(tex))
		{
			const auto& [diffuse_ID, specular_ID] = world.GetComponent<Components::TextureInfo>(entity);
			
			// textures
			// Set texture uniform value
			glUniform1i(glGetUniformLocation(renderInfo.shader_ID, "diffuse0"), 0);
			// Activate texture unit
			glActiveTexture(GL_TEXTURE0);
			// Bind texture to 
			glBindTexture(GL_TEXTURE_2D, diffuse_ID);

			glUniform1i(glGetUniformLocation(renderInfo.shader_ID, "specular0"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_ID);
		}

		// Draw VAO
		if (renderInfo.primitive_type == GL_POINTS)
			glDrawArrays(renderInfo.primitive_type, 0, renderInfo.size);
		else
		{
			/*if (primitive_type == GL_LINES)
				glClear(GL_DEPTH_BUFFER_BIT);*/
			glDrawElements(renderInfo.primitive_type, renderInfo.size, GL_UNSIGNED_INT, nullptr);
		}
			
	}
}

void RenderSystem::Clean()
{
	
}

