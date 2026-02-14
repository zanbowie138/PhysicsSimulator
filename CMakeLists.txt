cmake_minimum_required(VERSION 3.20)

project(PhysicsEngine)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the core library
add_subdirectory(src/core)
add_subdirectory(src/lua_engine)
add_subdirectory(src/app)
