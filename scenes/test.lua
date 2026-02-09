Utils.Log("test.lua scene initializing...")

-- Debug Lines for visualization
rays = CreateLines({
    name = "rays",
    capacity = 1000,
    shader = "basic",
    color = {0, 1, 0}
})

boxes = CreateLines({
    name = "boxes",
    capacity = 10000,
    shader = "basic",
    color = {1, 0, 0}
})

hits = CreateLines({
    name = "hits",
    capacity = 100,
    shader = "basic",
    color = {1, 1, 0}
})

-- Floor
floor = CreateFloor({
    scale = 10,
    shader = "default",
    texture = "planks.png",
    specular = "planksSpec.png"
})

-- Random Cubes
for i = 1, 100 do
    cube = CreateCube({
        position = {
            math.random(-2, 2),
            math.abs(math.random(-2, 2)),
            math.random(-2, 2)
        },
        scale = 0.1,
        shader = "flat",
        color = {math.random(), math.random(), math.random()}
    })
end

-- Light sphere
light = CreateSphere({
    position = { 0, 1, 0 },
    scale = 0.1,
    shader = "basic",
    color = { 1, 1, 1 }
})

-- Runtime state
local state = {
    time = 0,
    selectedEntity = nil,
    rayLines = nil,
    boxLines = nil,
    hitLines = nil
}

-- Initialize debug renderables
function OnInit()
    state.rayLines = Debug.GetLines("rays")
    state.boxLines = Debug.GetLines("boxes")
    state.hitLines = Debug.GetLines("hits")

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
        local t = math.sin(currentTime / 1000.0) * 0.5 + 0.5  -- Oscillate between 0 and 1
        local height = Utils.Lerp(1.0, 4.0, t)

        lightTransform.worldPos = vec3.new(
            math.sin(state.time / 2000.0) * 3.0,
            height,  -- Smooth height transition using Lerp
            math.cos(state.time / 2000.0) * 3.0
        )
    end
end

-- Mouse click handler
function OnClick(input, camera)
    -- Generate ray from screen to world
    local ray = Utils.ScreenPointToRay(input.mousePosNormalized, camera.cameraMatrix)

    -- Visualize ray
    state.rayLines:Clear()
    state.rayLines:PushRay(ray, 10)

    -- Query collision with scene
    local entity, hit = PhysicsSystem.tree:QueryRay(ray)

    state.hitLines:Clear()
    if hit then
        print("Hit entity: " .. tostring(entity))
        state.selectedEntity = entity

        -- Highlight bounding box
        local box = PhysicsSystem.tree:GetBoundingBox(entity)
        state.hitLines:PushBoundingBox(box)
    else
        print("No hit")
        state.selectedEntity = nil
    end
end

return {
    light = light
}
