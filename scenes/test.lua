Utils.Log("test.lua scene initializing...")

-- Debug Lines for visualization
rays = CreateLines({
    name = "rays",
    capacity = 1000,
    shader = "basic",
    color = {0, 1, 0}
})

hits = CreateLines({
    name = "hits",
    capacity = 10000,
    shader = "basic",
    color = {1, 0, 0}
})

boxes = CreateLines({
    name = "boxes",
    capacity = 10000,
    shader = "basic",
    color = {1, 1, 1}
})

-- Floor
floor = CreateFloor({
    scale = 10,
    shader = "default",
    texture = "planks.png",
    specular = "planksSpec.png"
})

-- Random Cubes
for _ = 1, 100 do
    cube = CreateCube({
        position = {
            math.random() * 8.0 - 4.0,
            math.random() * 3.0,
            math.random() * 8.0 - 4.0
        },
        scale = 0.1,
        shader = "flat",
        color = {math.random(), math.random(), math.random()}
    })
    PhysicsSystem.tree:AddToTree(cube)
end

-- Light sphere
light = CreateSphere({
    position = { 0, 1, 0 },
    scale = 0.1,
    shader = "basic",
    color = { 1, 1, 1 },
    castsShadow = false
})
PhysicsSystem.tree:AddToTree(light)

-- Runtime states
local state = {
    time = 0
}

local debugState = {
    rayLines = nil,
    boxLines = nil,
    hitLines = nil,
    showDynamicBoxes = false,
    showOnlyDynamicLeaf = false
}

local selectedState = {
    entity = nil,
    updatedPos = vec3({0, 0, 0}),
    changed = false,
}

-- Initialize debug renderables
function OnInit()
    debugState.rayLines = Debug.GetLines("rays")
    debugState.boxLines = Debug.GetLines("boxes")
    debugState.hitLines = Debug.GetLines("hits")

    -- Test custom Lua functions
    Utils.Log("Scene initialized with custom logging!")

    local startTime = Utils.GetTime()
    Utils.Log("after time")
    Utils.Log("Start time (ms): " .. tostring(startTime))

    -- Test Lerp and Clamp
    local lerpValue = Utils.Lerp(0, 10, 0.5)
    Utils.Log("Lerp(0, 10, 0.5) = " .. tostring(lerpValue))

    local clampValue = Utils.Clamp(15, 0, 10)
    Utils.Log("Clamp(15, 0, 10) = " .. tostring(clampValue))

    print("Scene initialized")
end

-- Update loop (called every frame)
function OnUpdate(dt, input, camera)
    state.time = state.time + dt

    -- Animate light using custom Lerp function for smooth transitions
    if world.HasTransform(light) then
        local lightTransform = world.GetTransform(light)

        -- Use GetTime and Lerp for smooth vertical oscillation
        local currentTime = Utils.GetTime()
        local t = math.sin(currentTime / 1000.0) * 0.5 + 0.5 -- Oscillate between 0 and 1
        local height = Utils.Lerp(1.0, 4.0, t)

        lightTransform.worldPos = vec3(
            math.sin(state.time / 2000.0) * 3.0,
            height, -- Smooth height transition using Lerp
            math.cos(state.time / 2000.0) * 3.0
        )
        local newPos = lightTransform.worldPos
        PhysicsSystem.tree:UpdateEntity(light, newPos)
    end

    if selectedState.entity and selectedState.changed then
        local entityTransform = world.GetTransform(selectedState.entity)
        print(string.format("Updating %d with position (%.2f, %.2f, %.2f)", selectedState.entity,
            selectedState.updatedPos:unpack()))
        entityTransform.worldPos = selectedState.updatedPos
        PhysicsSystem.tree:UpdateEntity(selectedState.entity, selectedState.updatedPos)
        selectedState.changed = false
    end

    debugState.boxLines:Clear()
    if debugState.showDynamicBoxes then
        debugState.boxLines:PushBoundingBoxes(PhysicsSystem.tree:GetAllBoxes(debugState.showOnlyDynamicLeaf))
    end
end

-- GUI panels (called between NewFrame and Render)
function OnGUI()
    GUI.Begin("Lua Script Config")
    if GUI.CollapsingHeader("Entity Info") then
        if selectedState.entity then
            GUI.Text("Entity ID: " .. tostring(selectedState.entity))
            local t = world.GetTransform(selectedState.entity)
            if GUI.DragFloat3("Position", selectedState.updatedPos, 0.1) then
                selectedState.changed = true
            end
        else
            GUI.Text("No entity selected.")
        end
    end

    if GUI.CollapsingHeader("Config") then
        if GUI.CollapsingHeader("Dynamic BVH Tree") then
            debugState.showDynamicBoxes = GUI.Checkbox("Show Bounding Boxes ##Dynamic", debugState.showDynamicBoxes)
            debugState.showOnlyDynamicLeaf = GUI.Checkbox("Show only leaf nodes ##Dynamic",
                debugState.showOnlyDynamicLeaf)
        end
    end
    GUI.End()
end

-- Mouse click handler
function OnClick(input, camera)
    -- Generate ray from screen to world
    local ray = Utils.ScreenPointToRay(input.mousePosNormalized, camera.cameraMatrix)

    -- Visualize ray
    debugState.rayLines:Clear()
    debugState.rayLines:PushRay(ray, 10)

    -- Query collision with scene
    local entity, hit = PhysicsSystem.tree:QueryRay(ray)
    local intersectedBoxes, anyHit = PhysicsSystem.tree:QueryRayCollisions(ray)

    debugState.hitLines:Clear()
    if anyHit then
        debugState.hitLines:PushBoundingBoxes(intersectedBoxes)
    end

    if hit then
        print("Hit entity: " .. tostring(entity))
        selectedState.entity = entity
        local t = world.GetTransform(selectedState.entity)
        selectedState.updatedPos = vec3({t.worldPos.x, t.worldPos.y, t.worldPos.z})
    else
        print("No hit")
        selectedState.entity = nil
    end
end

return {
    light = light,
    description = "ray casting, BVH, and entity selection showcase.",
    camera = {
        position = {0, 1, 7},
        fov = 45
    }
}
