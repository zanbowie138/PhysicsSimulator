project(CoreEngine)

set(SRC_FILES
        src/physics/DynamicTree.cpp
        src/physics/StaticTree.cpp
        src/renderer/RenderSystem.cpp
        src/glad.c
        src/stb.cpp
)

# Set variables
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
set(IMGUI_SOURCES
        ${DEPENDENCIES_DIR}/imgui/imgui.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_demo.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_draw.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_impl_glfw.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_impl_opengl3.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_tables.cpp
        ${DEPENDENCIES_DIR}/imgui/imgui_widgets.cpp
)

add_subdirectory(${DEPENDENCIES_DIR}/glfw-3.4)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${IMGUI_SOURCES})

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

target_include_directories(${PROJECT_NAME} PUBLIC ${DEPENDENCIES_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PUBLIC
        glfw
)
