---@meta

-- ============================================================
-- world table
-- ============================================================

---@class WorldAPI
local WorldAPI = {}

---@return integer entity
function WorldAPI.CreateEntity() end

---@param entity integer
---@return Transform
function WorldAPI.GetTransform(entity) end

---@param entity integer
---@return boolean
function WorldAPI.HasTransform(entity) end

---@type WorldAPI
world = nil

-- ============================================================
-- PhysicsSystem table
-- ============================================================

---@class PhysicsSystemAPI
---@field tree DynamicBBTree

---@type PhysicsSystemAPI
PhysicsSystem = nil

-- ============================================================
-- Debug table
-- ============================================================

---@class DebugAPI
local DebugAPI = {}

---@param name string
---@return Lines
function DebugAPI.GetLines(name) end

---@type DebugAPI
Debug = nil

-- ============================================================
-- Utils table
-- ============================================================

---@class UtilsAPI
local UtilsAPI = {}

---@param message string
---@param level? string "INFO"|"WARNING"|"ERROR" (default "INFO")
function UtilsAPI.Log(message, level) end

---@param a number
---@param b number
---@param t number Clamped to [0, 1]
---@return number
function UtilsAPI.Lerp(a, b, t) end

---@param value number
---@param min number
---@param max number
---@return number
function UtilsAPI.Clamp(value, min, max) end

---@return number Milliseconds since startup
function UtilsAPI.GetTime() end

---@param uv vec2 Normalized screen position [-1, 1]
---@param cameraMatrix mat4
---@return Ray
function UtilsAPI.ScreenPointToRay(uv, cameraMatrix) end

---@type UtilsAPI
Utils = nil

-- ============================================================
-- Scene creation helpers
-- ============================================================

---@class MeshConfig
---@field position? number[] {x, y, z}
---@field scale? number
---@field rotation? number[] {x, y, z} Euler angles in degrees
---@field shader? string "flat"|"basic"|"default"|"diffuse"
---@field color? number[] {r, g, b}

---@param cfg MeshConfig
---@return integer entity
function CreateCube(cfg) end

---@class FloorConfig
---@field scale? number
---@field shader? string
---@field texture? string Filename in res/textures/
---@field specular? string Specular map filename in res/textures/

---@param cfg FloorConfig
---@return integer entity
function CreateFloor(cfg) end

---@param cfg MeshConfig
---@return integer entity
function CreateSphere(cfg) end

---@class LinesConfig
---@field name string Required - key for Debug.GetLines()
---@field capacity? integer Default 1000
---@field shader? string Default "basic"
---@field color? number[] {r, g, b} Default {1, 1, 1}

---@param cfg LinesConfig
---@return integer entity
function CreateLines(cfg) end
