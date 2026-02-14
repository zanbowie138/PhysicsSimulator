#include "RenderSystem.h"

extern World world;

void RenderSystem::PreUpdate()
{
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::ShadowPass()
{
	if (!mShadowShaderID) return;

	glViewport(0, 0, ShadowMap::WIDTH, ShadowMap::HEIGHT);
	GL_FCHECK(glBindFramebuffer(GL_FRAMEBUFFER, mShadowMap.FBO));
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	glUseProgram(mShadowShaderID);
	glUniformMatrix4fv(glGetUniformLocation(mShadowShaderID, "lightSpaceMatrix"),
		1, GL_FALSE, glm::value_ptr(mLightSpaceMatrix));

	for (const auto& entity : mEntities) {
		const auto& ri = world.GetComponent<Components::RenderInfo>(entity);
		if (!ri.enabled || !ri.castsShadow || ri.primitive_type != GL_TRIANGLES) continue;

		auto& tr = world.GetComponent<Components::Transform>(entity);
		tr.CalculateModelMat();

		GL_FCHECK(glBindVertexArray(ri.VAO_ID));
		glUniformMatrix4fv(glGetUniformLocation(mShadowShaderID, "model"),
			1, GL_FALSE, glm::value_ptr(tr.modelMat));
		GL_FCHECK(glDrawElements(GL_TRIANGLES, ri.size, GL_UNSIGNED_INT, nullptr));
	}

	glCullFace(GL_BACK);
	GL_FCHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	int w, h;
	glfwGetFramebufferSize(mWindow, &w, &h);
	glViewport(0, 0, w, h);
}

void RenderSystem::Update() const
{
	assert(mWindow && "Window not set.");

	auto diffuse = world.GetComponentType<Components::DiffuseTextureInfo>();
	auto specular = world.GetComponentType<Components::SpecularTextureInfo>();

	// Bind shadow map globally (before entity loop)
	GL_FCHECK(glActiveTexture(GL_TEXTURE2));
	GL_FCHECK(glBindTexture(GL_TEXTURE_2D, mShadowMap.depthTexture));

	for (const auto& entity : mEntities)
	{
		const auto& renderInfo = world.GetComponent<Components::RenderInfo>(entity);

		if (!renderInfo.enabled) { continue; }

		// Update transform
		auto& transform = world.GetComponent<Components::Transform>(entity);
		transform.CalculateModelMat();

		// Bind vertex array
		GL_FCHECK(glBindVertexArray(renderInfo.VAO_ID));

		// Bind shader
		GL_FCHECK(glUseProgram(renderInfo.shader_ID));

		auto entitySignature = world.GetEntitySignature(entity);

		GL_FCHECK(glUniformMatrix4fv(glGetUniformLocation(renderInfo.shader_ID, "model"), 1, GL_FALSE, glm::value_ptr(transform.modelMat)));
		GL_FCHECK(glUniform3fv(glGetUniformLocation(renderInfo.shader_ID, "color"), 1, glm::value_ptr(renderInfo.color)));

		// Pass shadow uniforms (no-op for shaders that don't use them)
		GL_FCHECK(glUniform1i(glGetUniformLocation(renderInfo.shader_ID, "shadowMap"), 2));
		GL_FCHECK(glUniformMatrix4fv(
			glGetUniformLocation(renderInfo.shader_ID, "lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(mLightSpaceMatrix)));

		// Test if entity has a texture
		if (entitySignature.test(diffuse))
		{
			const auto& [diffuse_ID] = world.GetComponent<Components::DiffuseTextureInfo>(entity);

			// textures
			// Set texture uniform value
			GL_FCHECK(glUniform1i(glGetUniformLocation(renderInfo.shader_ID, "diffuse0"), 0));
			// Activate texture unit
			GL_FCHECK(glActiveTexture(GL_TEXTURE0));
			// Bind texture to
			GL_FCHECK(glBindTexture(GL_TEXTURE_2D, diffuse_ID));
		}

		// Test if entity has a texture
		if (entitySignature.test(specular))
		{
			const auto& [specular_ID] = world.GetComponent<Components::SpecularTextureInfo>(entity);

			GL_FCHECK(glUniform1i(glGetUniformLocation(renderInfo.shader_ID, "specular0"), 1));
			GL_FCHECK(glActiveTexture(GL_TEXTURE1));
			GL_FCHECK(glBindTexture(GL_TEXTURE_2D, specular_ID));
		}

		// Draw VAO
		if (renderInfo.primitive_type == GL_POINTS)
		{
			GL_FCHECK(glDrawArrays(renderInfo.primitive_type, 0, renderInfo.size));
		}
		else
		{
			GL_FCHECK(glDrawElements(renderInfo.primitive_type, renderInfo.size, GL_UNSIGNED_INT, nullptr));
		}
	}
}

void RenderSystem::PostUpdate()
{
	glfwSwapBuffers(mWindow);
	frames += 1;
}

void RenderSystem::Clean()
{
	mShadowMap.Clean();
}
