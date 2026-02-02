cmake_minimum_required(VERSION 3.10)
project(Lua)

# Core Lua source files
set(LUA_CORE_SOURCES
    lapi.c
    lcode.c
    lctype.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    llex.c
    lmem.c
    lobject.c
    lopcodes.c
    lparser.c
    lstate.c
    lstring.c
    ltable.c
    ltm.c
    lundump.c
    lvm.c
    lzio.c
)

# Lua standard library source files
set(LUA_LIB_SOURCES
    lauxlib.c
    lbaselib.c
    lcorolib.c
    ldblib.c
    liolib.c
    lmathlib.c
    loadlib.c
    loslib.c
    lstrlib.c
    ltablib.c
    lutf8lib.c
    linit.c
)

# Create static library
add_library(Lua STATIC ${LUA_CORE_SOURCES} ${LUA_LIB_SOURCES})

# Expose headers
target_include_directories(Lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link math library on Unix
if(UNIX)
    target_link_libraries(Lua PUBLIC m)
endif()
