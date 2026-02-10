#pragma once

#include <filesystem>

namespace Utils
{
	/**
	 * @brief Constructs resource path from base directory, subdirectory, and filename
	 * @param subdirectory Subdirectory within project (e.g., "/shaders/", "/res/textures/")
	 * @param filename File name
	 * @return Full path to resource
	 */
	inline std::string GetResourcePath(const std::string& subdirectory, const std::string& filename)
	{
		return std::filesystem::current_path().string() + subdirectory + filename;
	}
}
