-- Description: Rigidbody showcase.

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

-- Floor (box with physics)
floor = CreateBox({
    position = {0, -0.1, 0},
    dimensions = {10, 0.2, 10},
    shader = "default",
    texture = "planks.png",
    specular = "planksSpec.png"
})
PhysicsSystem.tree:AddToTree(floor)

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

    print("Scene initialized")
end

-- Update loop (called every frame)
function OnUpdate(dt, input, camera)
    state.time = state.time + dt

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
    camera = {
        position = {0, 1, 7},
        fov = 45
    }
}