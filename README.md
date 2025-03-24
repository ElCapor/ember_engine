# Ember Engine [![build](https://github.com/vsaint1/ember_engine/actions/workflows/build.yaml/badge.svg?branch=main)](https://github.com/vsaint1/ember_engine/actions/workflows/build.yaml/) [![docs](https://github.com/vsaint1/ember_engine/actions/workflows/docs.yaml/badge.svg?branch=main)](https://github.com/vsaint1/ember_engine/actions/workflows/docs.yaml)



**Ember Engine** is a lightweight and modular **2D game engine** written in **C/C++**, designed to be simple yet powerful. It aims to provide developers with a flexible cross-platform solution to build 2D games and applications.

### ✨ Features

- 🚀 **Cross-Platform**: 
  - Windows
  - Linux
  - macOS
  - Android
  - iOS
  - Web (via WebGL)

- 🎨 **Graphics Backend**:
  - OpenGL/ES (Windows, Linux, macOS, Android, Web)
  - Metal (macOS, iOS) **(coming soon)**

- 🛠️ **Lightweight Core**: Minimal dependencies for fast builds and portability.
- 🎮 **Input, Audio, and Texture Management**:  Simple and 
  extensible input, audio, and texture management.

---

### 📦 Dependencies

- [sdl3](https://github.com/libsdl-org/SDL) (Windowing, Events, Input, Audio, etc.)
- [sdl3_mixer](https://github.com/libsdl-org/SDL_mixer) (Audio mixer)
- [stb_image](https://github.com/nothings/stb) (Image loader)
- [stb_truetype](https://github.com/nothings/stb) (TrueType/SDF font loader)
- [glad](https://github.com/Dav1dde/glad) (OpenGL/ES Loader)
- [glm](https://github.com/g-truc/glm) (C++ math library)

---

### 📱 Supported Platforms Overview

| Platform | Backend        | Status              |
|----------|----------------|---------------------|
| Windows  | OpenGL 3.3     | ✅ Fully supported  |
| Linux    | OpenGL 3.3     | ✅ Fully supported  |
| MacOS    | OpenGL 3.3     | ✅ Fully supported  |
| Android  | OpenGL ES 3.0  | ✅ Fully supported  |
| iOS      | OpenGL ES 3.0  | ✅ Fully supported  |
| Web      | WebGL 3.0      | ✅ Fully supported  |
| iOS      | Metal          | 🚧 Coming soon      |
| MacOS    | Metal          | 🚧 Coming soon      |

> ⚠️ **Note:** Metal backend is planned for future versions.

---

### 📚 Documentation & Examples

Documentation and usage examples are coming soon.

---

### 🛠️ Build System

Ember Engine uses [cmake](https://cmake.org/) as its build system and supports the following toolchains:
- Visual Studio (Windows)
- GCC / Clang (Linux / macOS)
- Android NDK (Android)
- Xcode (macOS / iOS)
- Emscripten (WebGL)

---

### ⚠️ Disclaimer

This project was created for educational and personal purposes. It is **not** intended for commercial use.

---

### 📝 License

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT).