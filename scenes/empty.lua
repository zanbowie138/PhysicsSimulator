-- Description: An empty scene.

function OnInit()
    print("Empty scene initialized")
end

function OnUpdate(dt, input, camera)
end

function OnGUI()
end

light = CreateSphere({
    position = {0, 5, 0},
    scale = 0.1,
    shader = "basic",
    color = {1, 1, 1},
})

return {
    light = light,
}
