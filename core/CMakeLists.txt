set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set variables
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

find_package(glad CONFIG REQUIRED)

add_subdirectory(${DEPENDENCIES_DIR}/glfw-3.4)
include_directories(${DEPENDENCIES_DIR}/include)

set(IMGUI_SOURCES
        ${DEPENDENCIES_DIR}/include/imgui/imgui.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_demo.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_draw.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_impl_glfw.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_impl_opengl3.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_tables.cpp
        ${DEPENDENCIES_DIR}/include/imgui/imgui_widgets.cpp
)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_library(CoreEngine STATIC ${SRC_FILES} ${IMGUI_SOURCES})

target_link_libraries(CoreEngine PUBLIC
        glfw
        glad::glad
)