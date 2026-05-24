project(PhysicsEngine)

add_executable(${PROJECT_NAME}
    Main.cpp
    Application.cpp
    LayerStack.cpp
    layers/SimulationLayer.cpp
    layers/RenderingLayer.cpp
    layers/StatsLayer.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    CoreEngine
    LuaEngine
    -static
)
