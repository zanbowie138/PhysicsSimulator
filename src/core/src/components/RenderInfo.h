#pragma once

namespace Components
{
	struct RenderInfo {
		GLenum primitive_type;
		GLuint VAO_ID;
		GLuint shader_ID;
		size_t size;
		glm::vec3 color;
		bool enabled = true;
	};
}
