-- Description: Rigidbody showcase.

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
PhysicsSystem.AddRigidbody(floor, true)

-- Random Cubes
for i = 1, 100 do
    cube = CreateCube({
        position = {
            math.random() * 1,
            math.random() * 0.01 + 0.3*(i + 1),
            math.random() * 1
        },
        scale = 0.1,
        shader = "flat",
        color = {math.random(), math.random(), math.random()}
    })
    world.GetTransform(cube):SetRotationEuler(vec3(math.random() * 360, math.random() * 360, math.random() * 360))
    PhysicsSystem.tree:AddToTree(cube)
    PhysicsSystem.AddRigidbody(cube, false)
end

-- Light sphere
light = CreateSphere({
    position = { 1, 1, 0 },
    scale = 0.1,
    shader = "basic",
    color = { 1, 1, 1 },
    castsShadow = false
})
PhysicsSystem.tree:AddToTree(light)

-- Runtime state
local state = {
    time = 0
}

local debugState = {
    rayLines = nil,
    boxLines = nil,
    showDynamicBoxes = false,
    showOnlyDynamicLeaf = false
}

local dragState = {
    entity = nil,
    t = 0,          -- ray parameter at pick time (encodes drag depth)
    prevMouse = false
}

function OnInit()
    debugState.rayLines = Debug.GetLines("rays")
    debugState.boxLines = Debug.GetLines("boxes")

    print("Scene initialized")
end

function OnUpdate(dt, input, camera)
    state.time = state.time + dt

    -- Detect mouse-button press transition
    local justPressed = input.leftMouse and not dragState.prevMouse
    dragState.prevMouse = input.leftMouse

    if justPressed then
        local ray = Utils.ScreenPointToRay(input.mousePosNormalized, camera.cameraMatrix)
        local entity, hit = PhysicsSystem.tree:QueryRay(ray)
        if hit and world.HasRigidbody(entity) then
            local rb = world.GetRigidbody(entity)
            if not rb.isStatic then
                dragState.entity = entity
                -- Store ray parameter = projection of (center - origin) onto ray direction
                local c = world.GetTransform(entity).worldPos
                local o = ray.origin
                local d = ray.direction
                dragState.t = (c.x - o.x) * d.x + (c.y - o.y) * d.y + (c.z - o.z) * d.z
            end
        else
            dragState.entity = nil
        end
    end

    if not input.leftMouse then
        dragState.entity = nil
    end

    -- Apply spring force toward cursor while dragging
    if dragState.entity and input.leftMouse then
        local ray = Utils.ScreenPointToRay(input.mousePosNormalized, camera.cameraMatrix)
        local target = ray:GetPoint(dragState.t)
        local pos = world.GetTransform(dragState.entity).worldPos
        local rb = world.GetRigidbody(dragState.entity)

        local diff = target - pos
        rb:AddForce(diff * 50000.0 - rb.linearVelocity * 20.0)

        debugState.rayLines:Clear()
        debugState.rayLines:PushRay(ray, dragState.t)
    else
        debugState.rayLines:Clear()
    end

    debugState.boxLines:Clear()
    if debugState.showDynamicBoxes then
        debugState.boxLines:PushBoundingBoxes(PhysicsSystem.tree:GetAllBoxes(debugState.showOnlyDynamicLeaf))
    end
end

function OnGUI()
    GUI.Begin("Lua Script Config")
    if GUI.CollapsingHeader("Config") then
        if GUI.CollapsingHeader("Dynamic BVH Tree") then
            debugState.showDynamicBoxes = GUI.Checkbox("Show Bounding Boxes ##Dynamic", debugState.showDynamicBoxes)
            debugState.showOnlyDynamicLeaf = GUI.Checkbox("Show only leaf nodes ##Dynamic",
                debugState.showOnlyDynamicLeaf)
        end
    end
    GUI.End()
end

return {
    light = light,
    camera = {
        position = {0, 1, 7},
        fov = 45
    }
}
