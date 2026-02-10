#pragma once

// Primary template bloat source (28,911 lines)
#include <sol/sol.hpp>

// Heavy template libraries
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Core engine types with templates (order matters for dependencies)
#include "core/GlobalTypes.h"
#include "renderer/Camera.h"
#include "core/WindowManager.h"
#include "components/Components.h"
#include "core/World.h"
#include "physics/BoundingBox.h"
#include "physics/DynamicTree.h"
#include "math/Ray.h"
#include "renderables/Lines.h"
#include "renderables/Points.h"
#include "utils/Exceptions.h"
#include "utils/Logger.h"
#include "utils/Raycast.h"

// Standard library
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <tuple>
