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
for i = 1, 100 do
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
    color = { 1, 1, 1 }
})
PhysicsSystem.tree:AddToTree(light)

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
        local newPos = lightTransform.worldPos
        PhysicsSystem.tree:UpdateEntity(light, newPos)
    end

    state.boxLines:Clear()
    state.boxLines:PushBoundingBoxes(PhysicsSystem.tree:GetAllBoxes(false))
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
    local intersectedBoxes, anyHit = PhysicsSystem.tree:QueryRayCollisions(ray)

    state.hitLines:Clear()
    if anyHit then
        state.hitLines:PushBoundingBoxes(intersectedBoxes)
    end

    if hit then
        print("Hit entity: " .. tostring(entity))
        state.selectedEntity = entity
        SelectedEntity = entity
    else
        print("No hit")
        state.selectedEntity = nil
        SelectedEntity = nil
    end
end

return {
    light = light
}
