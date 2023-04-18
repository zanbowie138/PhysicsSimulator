#include"Texture.h"

Texture::Texture(const char* image, const char* texType, GLenum texFormat, GLuint slot, GLenum colorChannels, GLenum pixelType)
{
	// Assigns type of texture (ex: diffuse, specular)
	Texture::texType = texType;
	// Assigns OpenGL texture type (ex. GL_TEXTURE_2D)
	Texture::texFormat = texFormat;
	Texture::unit = slot;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);

	std::string localDir = "/Resources/Textures/";

	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load((std::filesystem::current_path().string() + localDir + image).c_str(), &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);

	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(texFormat, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texFormat, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texFormat, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texFormat, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texFormat, GL_TEXTURE_WRAP_T, GL_REPEAT);

	

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(texFormat, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Assigns the image to the OpenGL Texture object
	glTexImage2D(texFormat, 0, GL_RGBA, widthImg, heightImg, 0, colorChannels, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(texFormat);
	
	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texFormat, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = shader.GetUniformLocation(uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(texFormat, ID);
}

void Texture::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(texFormat, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}