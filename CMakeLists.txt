cmake_minimum_required(VERSION 3.20)

project(PhysicsEngine)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add the core library
add_subdirectory(src/core)
add_subdirectory(src/lua_engine)
add_subdirectory(src/app)
