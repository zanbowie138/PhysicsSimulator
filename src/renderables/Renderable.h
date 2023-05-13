#pragma once

#include "../core/GlobalTypes.h"
#include "../components/Components.h"
#include "../renderer/VAO.h"
#include "../core/ECS.h"

#include "../physics/BoundingBox.h"

extern ECSController ecsController;

// This class serves as a template to initialize OpenGL data and ECS.
class Renderable
{
public:
	VAO mVAO;
	Entity mEntityID = 999;

	GLuint ShaderID = 999;
	GLenum primitiveType = GL_TRIANGLES;

	void InitECS();
	void UpdateECSTransform();

	virtual void InitVAO() = 0;
	virtual size_t GetSize() = 0;

	Components::Transform transform;
};

inline void Renderable::InitECS()
{
	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ primitiveType, mVAO.ID, ShaderID, GetSize() });
}

inline void Renderable::UpdateECSTransform()
{
	ecsController.GetComponent<Components::Transform>(mEntityID) = transform;
}
