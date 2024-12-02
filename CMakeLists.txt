cmake_minimum_required(VERSION 3.20)

#if(${CMAKE_VERSION} VERSION_LESS 3.12)
#    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
#endif()

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PhysicsEngine)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Defines the BASE_DIR macro for the project as the root directory
#add_definitions(-DBASE_DIR="${EXECUTABLE_OUTPUT_PATH}")

# Add the core library
add_subdirectory(src/core)
add_subdirectory(src/app)
