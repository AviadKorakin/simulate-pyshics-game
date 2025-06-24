# 🚀 Demonstrate 2D Physics: Android + JNI + Box2D

![Physics Icon](https://img.icons8.com/color/48/000000/physics.png) ![Android Icon](https://img.icons8.com/color/48/000000/android-os.png) ![C++ Icon](https://img.icons8.com/color/48/000000/c-plus-plus-logo.png)

A modern Android app demonstrating a seamless JNI bridge to a native C++ Box2D physics engine, with a generic, JSON-driven level system. Drag, drop, and simulate real physics in a beautiful, interactive environment!

---

## 🏆 Features

- **Native Box2D Physics**: Realistic 2D physics simulation powered by the latest Box2D engine (C++).
- **JNI Bridge**: Clean, extensible Kotlin-to-C++ bridge for high-performance native calls.
- **Generic Level System**: Create and load any level using simple JSON files—no code changes needed!
- **Customizable Objects**: Define pillars, shelves, static blocks, obstacles, targets, and sources with colors, sprites, and physics properties.
- **Touch & Drag Gameplay**: Intuitive drag-and-launch controls with real-time feedback.
- **Score & Win Tracking**: Automatic scoring and win detection per level.
- **Beautiful Rendering**: Smooth, hardware-accelerated graphics with custom backgrounds and sprites.
- **Easy Extensibility**: Add new levels, objects, or physics tweaks with minimal effort.

---

## 🏗️ Project Architecture

```mermaid
%% Project architecture: Android, JNI, C++, JSON
---
<diagram will be rendered here>
```

---

## 📦 Getting Started

### 1. Clone the Project
```bash
git clone <your-repo-url>
cd demonstrate_2d_physics
```

### 2. Open in Android Studio
- Open the project root in Android Studio.
- Let Gradle sync and index the project.

### 3. Build & Run
- Connect an Android device or start an emulator.
- Click **Run** ▶️ in Android Studio.

---

## 🔗 JNI Bridge: How It Works

- The Kotlin object [`Box2DEngineNativeBridge`](app/src/main/java/com/aviadkorakin/demonstrate_2d_physics/Box2DEngineNativeBridge.kt) exposes native methods using `external fun`.
- On load, it calls `System.loadLibrary("demonstrate_2d_physics")` to load the C++ shared library.
- Native methods are implemented in [`NativeBridge.cpp`](app/src/main/cpp/NativeBridge.cpp) and registered via JNI.
- The bridge provides functions for world creation, stepping, body manipulation, collision handling, and more.

**Example:**
```kotlin
external fun createBox(x: Float, y: Float, halfWidth: Float, halfHeight: Float, density: Float, friction: Float, restitution: Float): Int
```

---

## ⚙️ Native Build Setup (CMake & Gradle)

- **CMake**: Native sources and Box2D are built via [`CMakeLists.txt`](app/src/main/cpp/CMakeLists.txt):
  - Adds Box2D as a subdirectory
  - Builds your native bridge and physics world
  - Links against Android log and native APIs

- **Gradle**: The app's [`build.gradle.kts`](app/build.gradle.kts) configures `externalNativeBuild`:
  ```kotlin
  externalNativeBuild {
      cmake {
          path = file("src/main/cpp/CMakeLists.txt")
          version = "3.22.1"
      }
  }
  ```

- **Adding Native Code**: Place new C++ files in `app/src/main/cpp/` and add them to `CMakeLists.txt`.

---

## 🧩 Creating Levels with JSON

- Levels are defined in `app/src/main/assets/levels/levelN.json`.
- Each file describes world gravity, bitmaps, and all objects (pillars, shelves, static blocks, targets, obstacles, source).
- **No code changes needed**—just add a new JSON file!

**Example:**
```json
{
  "levelName": "Level 1: Pillar Arc",
  "world": { "gravity": { "gx": 0, "gy": -9.8 } },
  "bitmaps": { "target": ["ic_target"], "source": ["ic_source"] },
  "objects": {
    "pillars": [ { "x": -1.5, "y": 2, "halfW": 0.2, "halfH": 2, "color": "#555555" } ],
    "targets": [ { "x": -3, "y": 4, "radius": 0.5, "score": 1, "spriteIndex": 0 } ],
    "source": { "x": 0, "y": 1, "radius": 0.5 }
  }
}
```

- See [`Level.kt`](app/src/main/java/com/aviadkorakin/demonstrate_2d_physics/level_manager/Level.kt) for the full schema.

---

## 🖼️ About `PhysicsView`

- Custom `SurfaceView` that renders the world, handles user input, and manages the game loop.
- Loads levels, draws objects, and interacts with the JNI bridge for all physics operations.
- Implements drag-and-launch, win detection, and dynamic rendering.

---

## 🛠️ Extending & Customizing

- **Add new levels**: Drop a new JSON file in `assets/levels/`.
- **Add new object types**: Extend the native bridge and update the JSON schema.
- **Change physics**: Tweak parameters in JSON or C++.
- **Add sprites**: Reference new drawable resources in your level JSON.

---

## 🎬 Demo Video

> [![Demo Video Placeholder](https://img.icons8.com/ios-filled/100/000000/video.png)](https://your-demo-link.com)
>
> _A video demonstration will be added here soon!_

---

## ⭐ Shine & Contribute

- Star this project if you like it!
- Contributions, issues, and suggestions are welcome.
- Make your own levels and share them!

---

## 📄 License

- This project uses Box2D (MIT License) and is open for educational and demo purposes. 