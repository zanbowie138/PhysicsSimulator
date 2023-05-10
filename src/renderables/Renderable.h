#pragma once

#include "../core/GlobalTypes.h"
#include "../components/Components.h"
#include "../renderer/VAO.h"
#include "../core/ECS.h"

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
	Components::Transform& GetTransform() const;

	virtual void InitVAO() = 0;
	virtual size_t GetSize() = 0;
private:
	Components::Transform transform;
};

inline void Renderable::InitECS()
{
	assert(GetSize() > 0 && "Size is 0");

	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ primitiveType, mVAO.ID, ShaderID, GetSize() });
}

inline Components::Transform& Renderable::GetTransform() const
{
	return ecsController.GetComponent<Components::Transform>(mEntityID);
}
