#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

namespace Components
{
	struct TextureInfo {
		GLuint diffuse_ID;
		GLuint specular_ID;
	};
}
