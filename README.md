# TSP Solver Project

## English

### Project Overview
This project is a **Traveling Salesman Problem (TSP) Solver** developed using C++ and the Qt framework. It provides an intuitive graphical user interface (GUI) for visualizing and solving the TSP using state-of-the-art optimization algorithms, including:
- **Ant Colony Optimization (ACO)**
- **Genetic Algorithm (GA)**
- **Particle Swarm Optimization (PSO)**

### Key Features
- **Real-Time Algorithm Visualization**: Dynamically displays the routes and progress of the algorithms, offering insights into their behavior.
- **Customizable Parameters**: Enables users to fine-tune algorithm parameters and adjust animation speed for better control.
- **Pause and Resume Functionality**: Allows users to pause and resume algorithm execution seamlessly.
- **Multi-Threaded Execution**: Ensures smooth and responsive UI interactions while computationally intensive algorithms run in the background.

### Build and Run Instructions
1. **Prerequisites**:
   - Qt 5 or 6
   - CMake
   - A C++ compiler (e.g., GCC, Clang, or MSVC)
2. **Build Steps**:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
3. **Run**:
   - Execute the generated binary (e.g., `TSP.exe` on Windows).

### Project Structure
- `src/`: Contains the core source code.
- `config/`: Includes configuration files for algorithm parameters.
- `ts/`: Stores translation files for internationalization.
- `build/`: Directory for build artifacts.

### License
This project is licensed under the MIT License.

---

## 中文

### 项目概述
本项目是一个使用 C++ 和 Qt 框架开发的**旅行商问题 (TSP) 求解器**。它提供了直观的图形用户界面 (GUI)，用于可视化和求解 TSP，支持以下先进的优化算法：
- **蚁群优化算法 (ACO)**
- **遗传算法 (GA)**
- **粒子群优化算法 (PSO)**

### 核心功能
- **实时算法可视化**：动态显示算法的路径和进度，帮助用户深入理解算法行为。
- **参数自定义**：允许用户微调算法参数并调整动画速度，以获得更好的控制。
- **暂停与继续功能**：支持算法执行的无缝暂停与继续。
- **多线程执行**：确保计算密集型算法在后台运行时，用户界面依然流畅响应。

### 构建与运行说明
1. **环境依赖**：
   - Qt 5 或 6
   - CMake
   - C++ 编译器（如 GCC、Clang 或 MSVC）
2. **构建步骤**：
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
3. **运行**：
   - 执行生成的二进制文件（如 Windows 上的 `TSP.exe`）。

### 项目结构
- `src/`：包含核心源代码。
- `config/`：包含算法参数的配置文件。
- `ts/`：存储用于国际化的翻译文件。
- `build/`：构建生成的文件目录。

### 许可证
本项目使用 MIT 许可证。