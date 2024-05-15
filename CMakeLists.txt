cmake_minimum_required(VERSION 3.7...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PhysicsEngine)

# Defines the BASE_DIR macro for the project as the root directory
add_definitions(-DBASE_DIR="${CMAKE_SOURCE_DIR}")

# Add the core library
add_subdirectory(${CMAKE_SOURCE_DIR}/core)

add_executable(${PROJECT_NAME} src/Main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/core/src
        ${CMAKE_SOURCE_DIR}/core/dependencies/include)
target_link_libraries(${PROJECT_NAME} PUBLIC CoreEngine)