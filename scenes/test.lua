-- Floor
floor = CreateFloor({
    scale = 10,
    shader = "default",
    texture = "planks.png",
    specular = "planksSpec.png"
})

-- Random cubes
--for i = 1, 100 do
--    cube = CreateCube({
--        position = {
--            math.random(-2, 2),
--            math.abs(math.random(-2, 2)),
--            math.random(-2, 2)
--        },
--        scale = 0.1,
--        shader = "flat",
--        color = {math.random(), math.random(), math.random()}
--    })
--end
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

-- Light sphere
light = CreateSphere({
    position = {0, 1, 0},
    scale = 0.1,
    shader = "basic",
    color = {1, 1, 1}
})
