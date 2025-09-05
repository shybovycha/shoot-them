# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Project Overview

ShootThem! is a simple shooting game written in modern C++ using OpenGL 4.6. The project consists of two main components:
- **Game**: The main shooting game with multiple levels
- **Editor**: Level editor for creating and modifying game levels (currently disabled in build)

## Build System

This project uses CMake with vcpkg for dependency management. The main dependencies include:
- OpenGL (GLEW, GLFW3)
- GLM (math library)
- TinyGLTF (3D model loading)
- STB (image loading)
- miniaudio (sound)
- fmt (formatting)

## Essential Commands

### Building the Project

```bash
# Clean build directory (optional)
cmake --build build --target clean

# Configure build files (requires VCPKG_HOME environment variable)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_HOME/scripts/buildsystems/vcpkg.cmake

# Build the game in Release mode
cmake --build build --config Release
```

### Running the Game

```bash
# On macOS
./build/game/game.app/Contents/MacOS/game

# On other platforms
./build/game/game
```

### Dependency Management

```bash
# Update vcpkg baseline after changing dependencies
vcpkg x-update-baseline
```

## Code Architecture

### Core Components

The game follows a scene-based architecture with the following key components:

1. **Application** (`game/src/core/application.hpp`): Main application class that manages the window, scene manager, and input handling
2. **SceneManager** (`game/src/core/scenemanager.hpp`): Manages scene transitions and lifecycle
3. **Scene** (`game/src/core/scene.hpp`): Base class for all game scenes
4. **WindowManager** (`game/src/core/windowmanager.hpp`): Handles window creation and OpenGL context
5. **Shader** (`game/src/core/shader.hpp`): Shader program management
6. **GLTFModel** (`game/src/core/gltfmodel.hpp`): 3D model loading and rendering

### Scene System

- Scenes are located in `game/src/scenes/`
- Each scene inherits from the base `Scene` class
- Scenes handle input events and rendering
- The game uses deferred rendering with multiple render passes

### Shader Pipeline

- Shaders are stored in `game/resources/shaders/`
- The game uses a deferred rendering pipeline with:
  - Forward render pass shader
  - Shading render pass shader
  - Post-processing effects (e.g., "drunk" effect)

### Resource Structure

```
game/resources/
├── fonts/       # UI fonts
├── models/      # 3D models
├── packs/       # Resource packs
├── shaders/     # GLSL shader files
├── sounds/      # Audio files
└── sprites/     # 2D sprites and textures
```

## Development Workflow

### Adding a New Scene

1. Create new scene class files in `game/src/scenes/`
2. Inherit from `Scene` base class
3. Register the scene in `SceneManager`
4. Add source files to `CMakeLists.txt`

### Modifying Shaders

Shaders are hot-reloadable. Files to modify:
- Vertex shaders: `*.vert` or `*.vert.glsl`
- Fragment shaders: `*.frag` or `*.frag.glsl`

### Working with Models

The game uses GLTF format for 3D models. Models should be placed in `game/resources/models/`.

## Platform-Specific Notes

### macOS
- The game builds as an app bundle
- Resources are automatically copied into the bundle

### Windows
- Ensure UTF-8 support is enabled (handled by CMake)
- DLLs are managed through vcpkg

### Linux
- Standard UNIX build process
- Resources are copied to the binary directory

## Current Development Status

The project is undergoing a major rework (2024-2025) transitioning from Irrlicht engine to modern OpenGL 4.6. The editor component is temporarily disabled during this transition.

### Active Development Areas

- Replacing 3D models with new artwork
- Implementing modern OpenGL rendering pipeline
- Adding UI improvements and HUD elements
- Improving post-processing effects

## Debugging Tips

### Shader Issues
- Check shader compilation errors in console output
- Verify shader file paths in resources directory
- Ensure GLSL version compatibility

### Build Issues
- Verify VCPKG_HOME environment variable is set
- Check vcpkg baseline in vcpkg.json matches installed versions
- Clean and rebuild if dependency issues occur

### Runtime Issues
- Resources must be in correct directory structure relative to binary
- Check OpenGL version compatibility (requires 4.6)
- Verify all required assets are present in resources directory
