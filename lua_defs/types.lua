---@meta

-- ============================================================
-- Math types
-- ============================================================

---@class vec2
---@field x number
---@field y number
local vec2 = {}

---@overload fun(): vec2
---@param x number
---@param y number
---@return vec2
function vec2.new(x, y) end

---@class vec3
---@field x number
---@field y number
---@field z number
local vec3 = {}

---@overload fun(): vec3
---@param x number
---@param y number
---@param z number
---@return vec3
function vec3.new(x, y, z) end

--- Opaque matrix type (4x4). Only used as a camera matrix parameter.
---@class mat4

-- ============================================================
-- Components
-- ============================================================

---@class Transform
---@field worldPos vec3
---@field scale vec3

-- ============================================================
-- Physics types
-- ============================================================

---@class Ray
---@field origin vec3
---@field direction vec3
local Ray = {}

---@overload fun(): Ray
---@param origin vec3
---@param direction vec3
---@return Ray
function Ray.new(origin, direction) end

---@param t number Distance along ray
---@return vec3
function Ray:GetPoint(t) end

---@class BoundingBox
---@field min vec3
---@field max vec3
local BoundingBox = {}

---@overload fun(): BoundingBox
---@param min vec3
---@param max vec3
---@return BoundingBox
function BoundingBox.new(min, max) end

-- ============================================================
-- Renderables
-- ============================================================

---@class Lines
local Lines = {}

function Lines:Clear() end

---@overload fun(self: Lines, origin: vec3, direction: vec3, length: number)
---@param ray Ray
---@param length number
function Lines:PushRay(ray, length) end

---@param box BoundingBox
function Lines:PushBoundingBox(box) end

---@param boxes BoundingBox[]
function Lines:PushBoundingBoxes(boxes) end

-- ============================================================
-- Input / Camera view (read-only in Lua)
-- ============================================================

---@class LuaInput
---@field leftMouse boolean
---@field rightMouse boolean
---@field w boolean
---@field a boolean
---@field s boolean
---@field d boolean
---@field space boolean
---@field ctrl boolean
---@field shift boolean
---@field mousePos vec2          Screen position in pixels
---@field mousePosNormalized vec2 Normalized screen position [-1, 1]

---@class LuaCameraView
---@field position vec3
---@field orientation vec3
---@field cameraMatrix mat4

-- ============================================================
-- Physics tree
-- ============================================================

---@class DynamicBBTree
local DynamicBBTree = {}

---@param ray Ray
---@return integer entity, boolean hit
function DynamicBBTree:QueryRay(ray) end

---@param entity integer
---@return BoundingBox
function DynamicBBTree:GetBoundingBox(entity) end

---@param entity integer
---@param bbox BoundingBox
function DynamicBBTree:InsertEntity(entity, bbox) end

---@param entity integer
function DynamicBBTree:RemoveEntity(entity) end

---@overload fun(self: DynamicBBTree, entity: integer, pos: vec3)
---@param entity integer
---@param bbox BoundingBox
function DynamicBBTree:UpdateEntity(entity, bbox) end

--- Insert entity using its pre-registered bounding box from the physics registry.
---@param entity integer
function DynamicBBTree:AddToTree(entity) end
