project(LuaEngine)

set(SRC_FILES
    src/lua/LuaRuntime.cpp
    src/lua/LuaBindings_Stable.cpp   # Rarely modified - compiles once
    src/lua/LuaBindings_Dynamic.cpp  # Frequently modified - recompiles often
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

# Precompiled headers for faster builds
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/pch.h"
)

# Public: Consumers need these paths
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Private: Only for implementation files
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# PUBLIC links: Headers expose both Core and Lua types
target_link_libraries(${PROJECT_NAME} PUBLIC
    CoreEngine
    Lua
)
