#pragma once

#include "../core/GlobalTypes.h"
#include "../components/Components.h"
#include "../renderer/VAO.h"
#include "../core/World.h"

extern World world;

// This class serves as a template to initialize OpenGL data and ECS.
class Renderable
{
public:
	virtual ~Renderable() = default;

	VAO mVAO;
	Entity mEntityID = 999;

	GLuint ShaderID = 999;
	GLenum primitiveType = GL_TRIANGLES;
	glm::vec3 mColor = glm::vec3(1.0f);

	void AddToECS();
	void UpdateECSTransform() const;

	void Scale(float scale) { transform.scale = glm::vec3(scale); }
	void Scale(glm::vec3 scale) { transform.scale = scale; }
	void SetPosition(glm::vec3 position) { transform.worldPos = position; }
	void SetRotation(glm::vec3 eulerRotation) { transform.SetRotationEuler(eulerRotation); }
	void SetColor(glm::vec3 color) { mColor = color; }

	virtual void InitVAO() = 0;
	virtual size_t GetSize() = 0;

	size_t mSize = 0;

	Components::Transform transform;
};

inline void Renderable::AddToECS()
{
	// Initialize entity
	mEntityID = world.CreateEntity();

	// Add components
	world.AddComponent(mEntityID, transform);
	world.AddComponent(mEntityID, Components::RenderInfo{ primitiveType, mVAO.ID, ShaderID, GetSize(), mColor});
}

inline void Renderable::UpdateECSTransform() const
{
	world.GetComponent<Components::Transform>(mEntityID) = transform;
}

