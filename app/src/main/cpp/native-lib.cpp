#include <jni.h>                    // JNI header for interfacing with Java/Kotlin
#include <vector>                   // C++ STL vector for storing body IDs
#include <box2d/box2d.h>            // Box2D C API umbrella header

// Keep one world handle and list of body handles alive between calls
static b2WorldId       worldId = b2_nullWorldId;  // Opaque handle to the Box2D world
static std::vector<b2BodyId> bodies;              // Dynamic array of body handles

// JNI: initialize or reset the physics world
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_initWorld(JNIEnv*, jobject) {
    // If a world already exists, destroy it to free resources
    if (B2_IS_NON_NULL(worldId)) {
        b2DestroyWorld(worldId);  // Teardown old world
        bodies.clear();           // Clear stored body handles
    }

    // 1) Configure world settings and create a new world with downward gravity
    b2WorldDef wdef = b2DefaultWorldDef();         // Get default world settings
    wdef.gravity = (b2Vec2){0.0f, -9.8f};          // Set gravity vector (m/s²)
    worldId = b2CreateWorld(&wdef);                // Create the world, store its ID

    // 2) Add a static ground body at y=0 so objects can bounce off
    {
        b2BodyDef groundDef = b2DefaultBodyDef();  // Default body settings
        groundDef.type     = b2_staticBody;        // Make body static (non-moving)
        groundDef.position = (b2Vec2){0.0f, 0.0f}; // Position at origin
        b2BodyId ground    = b2CreateBody(worldId, &groundDef);  // Create ground

        // Define a flat segment from x=-50 to +50 at ground level
        b2Segment seg{};
        seg.point1 = (b2Vec2){-50.0f, 0.0f};        // First endpoint
        seg.point2 = (b2Vec2){ 50.0f, 0.0f};        // Second endpoint

        // Define collision properties for the ground
        b2ShapeDef gsd = b2DefaultShapeDef();      // Default shape settings
        gsd.material.restitution = 0.8f;           // Bounciness coefficient
        gsd.material.friction    = 0.5f;           // Friction coefficient

        // Attach the segment shape to the ground body
        b2CreateSegmentShape(ground, &gsd, &seg);
    }
}

// JNI: advance the simulation by dt seconds
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepWorld(JNIEnv*, jobject, jfloat dt) {
    if (B2_IS_NON_NULL(worldId)) {
        b2World_Step(worldId, dt, 8);              // Step with 8 velocity/position iterations
    }
}

// JNI: get the X position (in meters) of body at index idx
extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyX(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return 0.0f;  // Bounds check
    return b2Body_GetPosition(bodies[idx]).x;                // Query X coordinate
}

// JNI: get the Y position (in meters) of body at index idx
extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyY(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return 0.0f;  // Bounds check
    return b2Body_GetPosition(bodies[idx]).y;                // Query Y coordinate
}

// JNI: get the rotation (radians) of body at index idx
extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyAngle(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return 0.0f;  // Bounds check
    b2Rot rot = b2Body_GetRotation(bodies[idx]);              // Get rotation struct
    return b2Rot_GetAngle(rot);                               // Convert to angle
}

// JNI: get the linear velocity X component of body at index idx
extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelX(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return 0.0f;  // Bounds check
    return b2Body_GetLinearVelocity(bodies[idx]).x;          // Query X velocity
}

// JNI: get the linear velocity Y component of body at index idx
extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelY(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return 0.0f;  // Bounds check
    return b2Body_GetLinearVelocity(bodies[idx]).y;          // Query Y velocity
}

// JNI: create a dynamic circle, returns its index in 'bodies'
extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createCircle(JNIEnv*, jobject,
                                                                                    jfloat x, jfloat y, jfloat radius,
                                                                                    jfloat density, jfloat friction, jfloat restitution) {
    // 1) Define and create the body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type     = b2_dynamicBody;                         // Dynamic body
    bd.position = (b2Vec2){x, y};                         // Set spawn position
    b2BodyId body = b2CreateBody(worldId, &bd);           // Create in world

    // 2) Define the circle geometry
    b2Circle circle{};
    circle.radius = radius;                               // Set radius

    // 3) Define physical properties
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density              = density;                    // Mass density
    sd.material.friction    = friction;                   // Friction
    sd.material.restitution = restitution;                // Bounciness

    // 4) Attach the shape to the body
    b2CreateCircleShape(body, &sd, &circle);

    // 5) Track and return the body index
    bodies.push_back(body);
    return (jint)bodies.size() - 1;
}

// JNI: create a dynamic box, returns its index in 'bodies'
extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createBox(JNIEnv*, jobject,
                                                                                 jfloat x, jfloat y,
                                                                                 jfloat halfW, jfloat halfH,
                                                                                 jfloat density, jfloat friction, jfloat restitution) {
    // 1) Define and create the body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type     = b2_dynamicBody;                         // Dynamic body
    bd.position = (b2Vec2){x, y};                         // Set spawn position
    b2BodyId body = b2CreateBody(worldId, &bd);           // Create in world

    // 2) Generate a box polygon via helper
    b2Polygon boxPoly = b2MakeBox(halfW, halfH);          // half-dimensions in meters

    // 3) Define physical properties
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density              = density;                   // Mass density
    sd.material.friction    = friction;                  // Friction
    sd.material.restitution = restitution;               // Bounciness

    // 4) Attach the box shape to the body
    b2CreatePolygonShape(body, &sd, &boxPoly);

    // 5) Track and return the body index
    bodies.push_back(body);
    return (jint)bodies.size() - 1;
}

// JNI: destroy the body at index idx and remove it from tracking
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyBody(JNIEnv*, jobject, jint idx) {
    if (idx < 0 || idx >= (jint)bodies.size()) return;    // Bounds check
    b2BodyId body = bodies[idx];                          // Get the handle
    b2DestroyBody(body);                                  // Remove from world
    bodies.erase(bodies.begin() + idx);                   // Remove from our vector
}