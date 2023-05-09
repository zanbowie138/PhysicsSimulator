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

	std::vector<GLuint> indices;

	void InitECS();
	Components::Transform& GetTransform() const;

	virtual void InitVAO() = 0;
private:
	Components::Transform transform;
};

inline void Renderable::InitECS()
{
	assert(!indices.empty() && "No indices");

	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ mVAO.ID, ShaderID, indices.size() });
}

inline Components::Transform& Renderable::GetTransform() const
{
	return ecsController.GetComponent<Components::Transform>(mEntityID);
}
