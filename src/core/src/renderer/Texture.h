#pragma once

#include <stb/stb_image.h>

#include <filesystem>
#include <optional>

#include "Shader.h"
#include <utils/PathUtils.h>

class Texture
{
public:
	GLuint ID{};
	GLenum texFormat;

	Texture();
	static inline std::optional<Texture> Load(const char* image, const GLenum texFormat, const GLenum colorChannels, const GLenum pixelType);

	// Deletes a texture
	inline void Delete() const;
private:
	inline void generateTexture(const unsigned char* bytes, GLenum texFormat, GLenum colorChannels, GLenum pixelType, int widthImg, int heightImg);
};

inline Texture::Texture() {}

inline std::optional<Texture> Texture::Load(const char* image, const GLenum texFormat, const GLenum colorChannels, const GLenum pixelType)
{
	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);

	std::string texturePath = Utils::GetResourcePath("/res/textures/", image);

	LOG(LOG_INFO) << "Loading texture: " << texturePath.c_str() << "\n";

	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(texturePath.c_str(), &widthImg,
	                                 &heightImg, &numColCh, 0);

	if (bytes == nullptr)
	{
		LOG(LOG_ERROR) << "Can't open texture " << image << " because: " << stbi_failure_reason() << "\n";
		return std::nullopt;
	}

	Texture texture;
	texture.texFormat = texFormat;
	texture.generateTexture(bytes, texFormat, colorChannels, pixelType, widthImg, heightImg);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	return texture;
}



inline void Texture::Delete() const
{
	GL_FCHECK(glDeleteTextures(1, &ID));
}

inline void Texture::generateTexture(const unsigned char* bytes, const GLenum texFormat, const GLenum colorChannels,
	const GLenum pixelType, const int widthImg, const int heightImg)
{
	// Generates an OpenGL texture object
	GL_FCHECK(glGenTextures(1, &ID));

	// Assigns the texture to a Texture Unit
	GL_FCHECK(glBindTexture(texFormat, ID));

	// Configures the type of algorithm that is used to make the image smaller or bigger
	GL_FCHECK(glTexParameteri(texFormat, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	GL_FCHECK(glTexParameteri(texFormat, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	// Configures the way the texture repeats (if it does at all)
	GL_FCHECK(glTexParameteri(texFormat, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_FCHECK(glTexParameteri(texFormat, GL_TEXTURE_WRAP_T, GL_REPEAT));

	// Assigns the image to the OpenGL Texture object
	GL_FCHECK(glTexImage2D(texFormat, 0, GL_RGBA, widthImg, heightImg, 0, colorChannels, pixelType, bytes));
	// Generates MipMaps
	GL_FCHECK(glGenerateMipmap(texFormat));

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	GL_FCHECK(glBindTexture(texFormat, 0));
}
