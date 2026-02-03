# PhysicsSimulator
An engine created with OpenGL and C++ that explores the graphics pipeline and computational physics from the ground up.

<br>

## Current features:
1. Rendering using Blinn-Phong shading with textures and custom shaders.
2. Uses an ECS (Entity Component System) for entity management and fast object manipulation.
3. STL model loading <a href="https://github.com/zanbowie138/STLFileReader">(using this custom loader)</a>
4. Broad-phase collision testing using both static and dynamic bounding volume hierarchies.
5. GUI created using [ImGUI](https://github.com/ocornut/imgui)
6. Lua-based scene scripting for declarative scene setup

## Build Requirements

- **CMake** 3.20 or higher
- **Ninja** build system
- **C++ Compiler** with C++17 support (MinGW-w64 recommended on Windows)
- **OpenGL** 3.3+ capable graphics card and drivers

All dependencies (GLFW, GLAD, GLM, ImGui, Lua, Sol2, stb_image) are bundled in `src/core/dependencies/`.

## Building

### Configure
```bash
cmake -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

### Build
```bash
cmake --build cmake-build-debug
```

### Run
The executable `PhysicsEngine.exe` (or `PhysicsEngine` on Linux) will be created in the project root directory.

**Important:** Run from the project root directory to ensure proper resource loading (shaders, textures, models, scenes).

```bash
./PhysicsEngine.exe
```

## Videos:
https://github.com/user-attachments/assets/e6971172-b453-4f50-bc1d-022fda9575d7

https://github.com/user-attachments/assets/2bcb06c3-9c95-4442-a090-bb1b4dd07bf6

https://github.com/user-attachments/assets/e65ffbf4-31cf-483c-980f-8abd61727a5b






## Future Additions:
1. Rigidbody collisions
2. Cloth, fluid simulations
3. More complex model loading
4. Monte Carlo style raytracing
5. Multithreading
