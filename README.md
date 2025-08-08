# What can you make with the Ocero Engine?
![Gameplay](https://github.com/BrandonLQuintin/Quintin-OpenGL-Project/assets/104369655/688b4c51-f43c-4059-84bf-60172a853b2a)

# A Game Engine made from scratch in C++ and OpenGL!
![Screenshot 2024-03-07 212524](https://github.com/BrandonLQuintin/Quintin-OpenGL-Project/assets/104369655/b184681c-8144-431b-b95e-373e22e6d492)

# Cool Facts About The Game
1. This engine always runs above 1,000 FPS on a steam deck (you must run in native resolution on an external 1080p monitor and steam proton).
2. This engine always runs above 2,000 FPS at the Elms College computer lab (with the RTX 2070-Super, and modern i7 processor), and often runs above 2,500 FPS. Occasionally, it even reaches above 3,000 FPS.
3. The total file size of the engine is 25.5 MB, but without .wav music files, the engine is 7.54 MB.
4. 4,051 lines of code was written to make this engine (as of release version 1.0.0).

# Build & Run (CMake)

The project uses portable CMake with per-config build directories: build/Debug and build/Release. Dependencies are resolved system-first with FetchContent fallback (GLFW, GLM, SFML audio).

## Requirements (minimal)
- CMake 3.16+ and a C++17 compiler
- OpenGL drivers
- Linux audio runtime libs: OpenAL, libsndfile, X11
- Optional (only if LLM features enabled): libcurl

Arch Linux example:
- sudo pacman -S cmake gcc make ninja openal libsndfile curl

## Quick Start (VS Code)
1) Install extensions: CMake Tools and CodeLLDB.
2) Select a config and press F5:
   - OceroEngine (LLDB Debug): builds to build/Debug and runs build/Debug/bin/OceroEngine
   - OceroEngine (LLDB Release): builds to build/Release and runs build/Release/bin/OceroEngine
3) Output appears in the Debug Console. Assets/shaders are copied next to the executable.

## Command Line Build
- Debug
  - cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug
  - cmake --build build/Debug -j
  - ./build/Debug/bin/OceroEngine
- Release
  - cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release
  - cmake --build build/Release -j
  - ./build/Release/bin/OceroEngine
- Clean builds
  - rm -rf build
  - or to re-fetch deps only: rm -rf build/_deps

## CMake options
- OceroEngine_ENABLE_LLM=ON|OFF
  - Disable to remove libcurl requirement entirely.
-- OceroEngine_ENABLE_LTO=ON|OFF
  - Enable link-time optimization where supported.

## Troubleshooting
- SFML fetched but link errors about audio/system:
  - Ensure OpenAL and libsndfile are installed (e.g., pacman -S openal libsndfile on Arch)
- cURL not found during configure:
  - Either install curl dev libs (pacman -S curl) or disable LLM: -DOceroEngine_ENABLE_LLM=OFF
- Stale third-party sources:
  - Remove build/_deps and reconfigure to force re-fetch
- Wayland vs X11:
  - GLFW/SFML can build for either; ensure the matching dev packages are present for your session

- GLX/EGL: Failed to create OpenGL context at runtime (Linux)
  - Symptom (from console):
    - GLX: Failed to create context: BadValue (integer parameter out of range for operation)
    - or EGL: Failed to initialize EGL: EGL cannot access a requested resource
  - Causes: Missing/incorrect OpenGL drivers, mismatched X11/GLX stack, or hybrid-GPU offloading not configured.
  - Diagnostics:
    - Ensure you're on a graphical session: echo $XDG_SESSION_TYPE; echo $DISPLAY
    - Install mesa-demos and query GLX:
      - Arch: sudo pacman -S mesa-demos
      - Run: glxinfo -B
    - Quick test: glxgears -info
  - Fixes (Arch examples):
    - Intel/AMD: sudo pacman -S mesa libglvnd
    - NVIDIA (proprietary): sudo pacman -S nvidia nvidia-utils libglvnd
    - Reboot after driver install/update.
    - Hybrid NVIDIA Optimus: try PRIME offload
      - prime-run ./build/<type>/bin/OceroEngine
      - or: __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ./build/<type>/bin/OceroEngine
    - Wayland users: prefer X11 session for GLX, or ensure EGL path is available.
  - Notes:
    - The engine attempts several GLFW context configurations automatically (GL 3.3/3.2 core, 3.0 any, default hints, and EGL). If all fail, it's a system driver/GLX issue rather than a code problem.

## Windows (legacy)
The project previously targeted Code::Blocks (MinGW). CMake is now the recommended cross-platform build. If you still need the legacy setup, see earlier commit history.

## Credits
With terrain generation and collision, I followed ThinMatrix's youtube tutorials for OpenGL (tutorial 14, 21, and 22)
- https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=14
- https://www.youtube.com/watch?v=O9v6olrHPwI&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=21
- https://www.youtube.com/watch?v=6E2zjfzMs7c&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=22
