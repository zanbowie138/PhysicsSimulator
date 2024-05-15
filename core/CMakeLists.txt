set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set variables
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

find_package(glad CONFIG REQUIRED)

add_subdirectory(${DEPENDENCIES_DIR}/glfw-3.4)
include_directories(${DEPENDENCIES_DIR}/include)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_library(CoreEngine STATIC ${SRC_FILES})

target_link_libraries(CoreEngine PUBLIC
        glfw
        glad::glad
)