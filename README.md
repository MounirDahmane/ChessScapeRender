# ChessScapeRender ♟️

**ChessScapeRender** is a high-performance 3D Chess visualization engine built with **C++17** and **OpenGL 3.3 Core Profile**. It features an optimized asset loading pipeline using **Assimp**, a custom shader management system with uniform caching, and a responsive camera system.

<p align="center">
  <img src="docs/gameplay.gif" alt="Gameplay" width="600" />
</p>

---

## 🚀 Key Features

### Modern C++ Architecture
- Fully refactored into a clean `include/` and `src/` structure.
- Uses **RAII** principles.

### High-Performance Rendering
- **Ninja Build System** for fast incremental builds.
- **Uniform Caching** to minimize CPU-to-GPU overhead by caching shader locations.
- **Batch Processing** with optimized vertex attribute layouts and drawing calls.

### Advanced Graphics
- **Environment Mapping:** Real-time reflections on chess pieces using Cubemaps.
- **Stencil Outlining:** Support for piece selection and highlighting.
- **Asset Pipeline:** Loading of `.obj` and `.gltf` models using Assimp.

### Interactive Camera
- 6-DOF free-look camera.
- Smooth mouse movement.
- Adjustable FOV with scrolling.

---

## 🛠️ Tech Stack

| Component | Library |
| :--- | :--- |
| Graphics API | OpenGL 3.3 (Core Profile) |
| Windowing/Input | GLFW 3 |
| GL Loader | GLAD |
| Math | GLM (OpenGL Mathematics) |
| Asset Loading | Assimp (Open Asset Import Library) |
| Texture Loading | stb_image |
| Build System | CMake & Ninja |

---

## 📥 Installation & Setup

### Prerequisites (Linux/Ubuntu)

Ensure the required development libraries are installed:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build libglfw3-dev libassimp-dev libglm-dev
````

### Building the Project

1. Clone the repository:

```bash
git clone https://github.com/MounirDahmane/ChessScapeRender.git
cd 3D-Chess-Board
```

2. Configure the build:

```bash
chmod +x config.sh run.sh
./config.sh
```

3. Build and run:

```bash
./run.sh
```

---

## 🎮 Controls

* **W / A / S / D** — Move the camera (Forward / Left / Back / Right)
* **Mouse Movement** — Look around the scene
* **Scroll Wheel** — Adjust Field of View (Zoom)
* **F / T** — Lock / Unlock camera orientation
* **R** — Reset camera to default position and rotation
* **M** — Release mouse (show cursor and disable camera rotation)
* **N** — Capture mouse (hide cursor and enable camera look)
* **ESC** — Exit application

---

## 📁 Project Structure

```text
.
├── include/               # Header files (.h)
├── src/                   # Implementation files (.cpp)
├── resources/             # Shaders, Textures, and 3D Models
├── dependencies/          # Third-party libraries (GLAD, STB)
├── docs/                  # Documentation and media (gameplay gif)
├── build/                 # Compiled binaries and object files
├── CMakeLists.txt         # Main CMake build configuration
├── .gitignore             # Git ignored files and directories
├── LICENSE                # Project license
├── README.md              # Project documentation
├── format.sh              # Code formatting script
├── config.sh              # Build configuration script
└── run.sh                 # Build and execute script
└── .clang-format
```

---

## 📝 Developer Notes: Refactoring & Optimization

* **Split Compilation:** Implementation moved to `.cpp` files to reduce compile times.
* **Memory Safety:** Implemented the **Rule of Five** for GPU resource managing classes to prevent double deletion and segmentation faults.
* **Header Hygiene:** Fixed OpenGL and GLAD include order conflicts for cross-platform compatibility.
* **Resource Management:** Automated resource syncing to the build directory via CMake `POST_BUILD` commands.

```
```
