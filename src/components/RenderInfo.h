#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

namespace Components
{
	struct RenderInfo {
		GLuint VAO_ID;
		GLuint shader_ID;
		GLuint indices;
	};
}
