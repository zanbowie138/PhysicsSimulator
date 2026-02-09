#include <glad/glad.h>
#include <lua_engine/LuaRuntime.h>
#include "core/World.h"
// #include "physics/DynamicTree.h"
#include "renderables/Lines.h"
#include "renderables/Points.h"
#include "utils/Logger.h"
#include "utils/PathUtils.h"
#include "utils/Exceptions.h"

#include "scene/SceneHelper.h"
#include "scene/helpers/CubeHelper.h"
#include "scene/helpers/FloorHelper.h"
#include "scene/helpers/SphereHelper.h"
#include "scene/helpers/LinesHelper.h"

// Constructor and destructor must be defined in .cpp where SceneHelper is complete
LuaRuntime::LuaRuntime() = default;
LuaRuntime::~LuaRuntime() = default;

void LuaRuntime::Initialize(World& world, Physics::DynamicBBTree& tree,
                           const std::unordered_map<std::string, GLuint>& shaders) {
    LOG(LOG_INFO) << "Initializing Lua runtime\n";

    // Store shader map for fallback scene
    shaderMap = shaders;

    // Open standard Lua libraries
    lua.open_libraries(sol::lib::base, sol::lib::math);
    LOG(LOG_INFO) << "Opened Lua standard libraries\n";

    // Bind all engine APIs
    LuaBindings::BindCore(lua, world);
    LOG(LOG_INFO) << "Bound core types (vec2, vec3, Transform, Rigidbody, RenderInfo, World)\n";

    LuaBindings::BindPhysics(lua, tree);
    LOG(LOG_INFO) << "Bound physics types (Ray, BoundingBox, DynamicBBTree, Utils)\n";

    LuaBindings::BindInputAndCamera(lua);
    LOG(LOG_INFO) << "Bound input and camera types\n";

    // Register scene creation helpers
    sceneHelpers.push_back(std::make_unique<SceneImporterInternal::CubeHelper>());
    sceneHelpers.push_back(std::make_unique<SceneImporterInternal::FloorHelper>());
    sceneHelpers.push_back(std::make_unique<SceneImporterInternal::SphereHelper>());
    sceneHelpers.push_back(std::make_unique<SceneImporterInternal::LinesHelper>(*this));

    for (const auto& helper : sceneHelpers) {
        SceneImporterInternal::SceneHelper* helperPtr = helper.get();
        std::string helperName = helper->GetName();
        lua.set_function(helperName, [helperPtr, &world, &shaders, helperName](sol::table cfg) -> Entity {
            try {
                Entity id = helperPtr->Create(cfg, world, shaders);
                return id;
            } catch (const std::exception& e) {
                LOG(LOG_ERROR) << "Scene helper '" << helperName << "' failed: " << e.what() << "\n";
                throw;
            }
        });
        LOG(LOG_INFO) << "Registered scene helper: " << helperName << "\n";
    }

    callbacksRegistered = true;
    LOG(LOG_INFO) << "Lua runtime initialized successfully\n";
}

bool LuaRuntime::LoadScene(const std::string& filename, std::string& outErrorMsg) {
    if (!callbacksRegistered) {
        outErrorMsg = "LuaRuntime not initialized. Call Initialize() first.";
        LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
        return false;
    }

    LOG(LOG_INFO) << "Loading scene: " << filename << "\n";

    // Bind debug renderables after Initialize
    LuaBindings::BindDebugRendering(lua, debugLines, debugPoints);
    LOG(LOG_INFO) << "Bound " << debugLines.size() << " debug lines and "
                  << debugPoints.size() << " debug points\n";

    try {
        // Execute scene script
        std::string fullPath = Utils::GetResourcePath("/scenes/", filename);
        LOG(LOG_INFO) << "Executing scene script: " << fullPath << "\n";

        sol::load_result loadResult = lua.load_file(fullPath);
        if (!loadResult.valid()) {
            sol::error err = loadResult;
            outErrorMsg = std::string("Lua syntax error: ") + err.what();
            LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
            return false;
        }

        sol::protected_function_result result = loadResult();
        if (!result.valid()) {
            sol::error err = result;
            outErrorMsg = std::string("Lua error: ") + err.what();
            LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
            return false;
        }

        if (!result.return_count() || result.get_type() != sol::type::table) {
            outErrorMsg = "Scene file must return a table with 'light' field";
            LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
            return false;
        }

        sol::table sceneTable = result;
        if (!sceneTable["light"].valid()) {
            outErrorMsg = "Scene table must contain 'light' field with light entity ID";
            LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
            return false;
        }

        // Extract light entity from scene return value
        sol::optional<Entity> lightID = sceneTable["light"];
        if (lightID) {
            lightEntity = lightID.value();
            LOG(LOG_INFO) << "Light entity loaded: " << lightEntity << "\n";
        } else {
            outErrorMsg = "Scene did not provide valid 'light' entity";
            LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
            return false;
        }

        LOG(LOG_INFO) << "Scene loaded successfully\n";
        return true;

    } catch (const sol::error& e) {
        outErrorMsg = std::string("Lua error: ") + e.what();
        LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
        return false;
    } catch (const SceneException& e) {
        outErrorMsg = std::string("Scene error: ") + e.what();
        LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
        return false;
    } catch (const std::exception& e) {
        outErrorMsg = std::string("Unexpected error: ") + e.what();
        LOG(LOG_ERROR) << "Scene load failed: " << outErrorMsg << "\n";
        return false;
    }
}

void LuaRuntime::RegisterDebugLines(const std::string& name, Lines* lines) {
    debugLines[name] = lines;
}

void LuaRuntime::RegisterDebugPoints(const std::string& name, Points* points) {
    debugPoints[name] = points;
}

void LuaRuntime::CallOnInit() {
    sol::optional<sol::protected_function> callback = lua["OnInit"];
    if (callback) {
        LOG(LOG_INFO) << "Calling OnInit callback\n";
        try {
            sol::protected_function_result result = callback.value()();
            if (!result.valid()) {
                sol::error err = result;
                LOG(LOG_ERROR) << "OnInit error: " << err.what() << "\n";
            } else {
                LOG(LOG_INFO) << "OnInit completed successfully\n";
            }
        } catch (const std::exception& e) {
            LOG(LOG_ERROR) << "OnInit exception: " << e.what() << "\n";
        }
    } else {
        LOG(LOG_INFO) << "No OnInit callback defined\n";
    }
}

void LuaRuntime::CallOnUpdate(float dt, const LuaBindings::LuaInput& input,
                             const LuaBindings::LuaCameraView& camera) {
    sol::optional<sol::protected_function> callback = lua["OnUpdate"];
    if (callback) {
        try {
            sol::protected_function_result result = callback.value()(dt, input, camera);
            if (!result.valid()) {
                sol::error err = result;
                LOG(LOG_ERROR) << "OnUpdate error: " << err.what() << "\n";
            }
        } catch (const std::exception& e) {
            LOG(LOG_ERROR) << "OnUpdate exception: " << e.what() << "\n";
        }
    }
}

void LuaRuntime::CallOnClick(const LuaBindings::LuaInput& input,
                            const LuaBindings::LuaCameraView& camera) {
    sol::optional<sol::protected_function> callback = lua["OnClick"];
    if (callback) {
        LOG(LOG_INFO) << "Calling OnClick callback\n";
        try {
            sol::protected_function_result result = callback.value()(input, camera);
            if (!result.valid()) {
                sol::error err = result;
                LOG(LOG_ERROR) << "OnClick error: " << err.what() << "\n";
            }
        } catch (const std::exception& e) {
            LOG(LOG_ERROR) << "OnClick exception: " << e.what() << "\n";
        }
    }
}

bool LuaRuntime::LoadFallbackScene(std::string& outErrorMsg) {
    try {
        LOG(LOG_INFO) << "Loading fallback scene\n";

        // Get floor helper to create floor
        SceneImporterInternal::SceneHelper* floorHelper = nullptr;
        for (const auto& helper : sceneHelpers) {
            if (helper->GetName() == "CreateFloor") {
                floorHelper = helper.get();
                break;
            }
        }

        if (!floorHelper) {
            outErrorMsg = "FloorHelper not found";
            LOG(LOG_ERROR) << "Fallback scene failed: " << outErrorMsg << "\n";
            return false;
        }

        // Create floor config
        sol::table floorCfg = lua.create_table();
        floorCfg["scale"] = 10.0f;
        floorCfg["shader"] = "flat";

        // Create floor entity
        extern World world;
        Entity floor = floorHelper->Create(floorCfg, world, shaderMap);

        // Get sphere helper to create light
        SceneImporterInternal::SceneHelper* sphereHelper = nullptr;
        for (const auto& helper : sceneHelpers) {
            if (helper->GetName() == "CreateSphere") {
                sphereHelper = helper.get();
                break;
            }
        }

        if (!sphereHelper) {
            outErrorMsg = "SphereHelper not found";
            LOG(LOG_ERROR) << "Fallback scene failed: " << outErrorMsg << "\n";
            return false;
        }

        // Create light config
        sol::table lightCfg = lua.create_table();
        sol::table lightPos = lua.create_table();
        lightPos[1] = 0.0f;
        lightPos[2] = 5.0f;
        lightPos[3] = 0.0f;
        lightCfg["position"] = lightPos;
        lightCfg["scale"] = 0.1f;
        lightCfg["shader"] = "basic";
        sol::table lightColor = lua.create_table();
        lightColor[1] = 1.0f;
        lightColor[2] = 1.0f;
        lightColor[3] = 1.0f;
        lightCfg["color"] = lightColor;

        // Create light entity
        lightEntity = sphereHelper->Create(lightCfg, world, shaderMap);

        LOG(LOG_INFO) << "Loaded fallback scene (floor + light)\n";
        return true;

    } catch (const std::exception& e) {
        outErrorMsg = std::string("Fallback scene failed: ") + e.what();
        LOG(LOG_ERROR) << outErrorMsg << "\n";
        return false;
    }
}

void LuaRuntime::TakeOwnership(std::unique_ptr<Lines> lines) {
    ownedLines.push_back(std::move(lines));
}

void LuaRuntime::TakeOwnership(std::unique_ptr<Points> points) {
    ownedPoints.push_back(std::move(points));
}
