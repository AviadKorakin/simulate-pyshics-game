# Specify the minimum required CMake version; this ensures features used in this file are available.
cmake_minimum_required(VERSION 3.22.1)

# Declare the project name; this defines the variable ${PROJECT_NAME} as "demonstrate_2d_physics".
project("demonstrate_2d_physics")

# 1. Build the Box2D engine by descending into its directory.
#    This expects a subfolder named "box2d" containing its own CMakeLists.txt.
add_subdirectory(box2d)

# 2. Define your shared library target.
#    The name is taken from ${PROJECT_NAME}, and it will build native-lib.cpp into a .so.
add_library(${CMAKE_PROJECT_NAME}
        SHARED
        NativeBridge.cpp
        PhysicsWorld.cpp
        BodyFactory.cpp
        Extras.cpp


)

# 3. Locate the Android log library (liblog.so) on the system.
#    The result is stored in the variable log-lib for later linking.
find_library(log-lib
        log)

# 4. Include the Box2D headers so that *.cpp can find Box2D's API.
#    CMAKE_CURRENT_SOURCE_DIR is the directory containing this CMakeLists.txt.
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/box2d/include)

# 5. Link your shared library against:
#    - box2d:   the engine you built via add_subdirectory
#    - ${log-lib}: the Android logging library
#    - android: the Android native APIs (for ANativeActivity, etc.)
target_link_libraries(${CMAKE_PROJECT_NAME}
        box2d
        ${log-lib}
        android)