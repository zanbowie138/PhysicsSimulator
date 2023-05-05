#pragma once
#include <bitset>
#include "glm/glm.hpp"

constexpr unsigned int MAX_ENTITIES = 500;
constexpr unsigned int MAX_COMPONENTS = 10;

// EntityID
using Entity = unsigned int;

// Basically an array of bools identifying what components are being used
using Signature = std::bitset<MAX_COMPONENTS>;

// Component ID
using ComponentType = uint8_t;


// Input
enum class InputButtons
{
	MOUSE,
	W,
	A,
	S,
	D,
	SPACE,
	CONTROL,
	SHIFT,
	ENUM_LENGTH
};