#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

namespace Components
{
	struct DiffuseTextureInfo {
        GLuint diffuse_ID;
    };
	struct SpecularTextureInfo {
        GLuint specular_ID;
    };
}
