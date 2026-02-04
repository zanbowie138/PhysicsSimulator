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
    print("Scene initialized")
end

-- Update loop (called every frame)
function OnUpdate(dt, input, camera)
    state.time = state.time + dt

    -- Animate light in circular pattern
    local lightTransform = world:GetTransform(light)
    lightTransform.worldPos = vec3(
        math.sin(state.time / 2000.0) * 3.0,
        3.0,
        math.cos(state.time / 2000.0) * 3.0
    )
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
