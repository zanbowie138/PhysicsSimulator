#pragma once

#include "../core/GlobalTypes.h"
#include "../components/Components.h"
#include "../renderer/VAO.h"
#include "../core/ECS/ECSController.h"

extern ECSController ecsController;

// This class serves as a template to initialize OpenGL data and ECS.
class Renderable
{
public:
	VAO mVAO;
	Entity mEntityID = 999;

	GLuint ShaderID = 999;
	GLenum primitiveType = GL_TRIANGLES;
	glm::vec3 mColor = glm::vec3(1.0f);

	void AddToECS();
	void UpdateECSTransform() const;

	virtual void InitVAO() = 0;
	virtual size_t GetSize() = 0;

	size_t mSize = 0;

	Components::Transform transform;
};

inline void Renderable::AddToECS()
{
	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ primitiveType, mVAO.ID, ShaderID, GetSize(), mColor});
}

inline void Renderable::UpdateECSTransform() const
{
	ecsController.GetComponent<Components::Transform>(mEntityID) = transform;
}
