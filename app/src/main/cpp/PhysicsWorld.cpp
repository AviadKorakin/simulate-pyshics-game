#include "PhysicsWorld.h"
#include "BodyFactory.h"
#include <box2d/box2d.h>

// Returns the singleton instance of PhysicsWorld
PhysicsWorld& PhysicsWorld::instance() {
    static PhysicsWorld inst;            // Create one static instance
    return inst;                         // Always return the same instance
}

// Constructor: initialize worldId_ to invalid
PhysicsWorld::PhysicsWorld()
        : worldId_(b2_nullWorldId) {}     // No world created yet

// Destructor: if a world exists, destroy it
PhysicsWorld::~PhysicsWorld() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);       // Clean up native Box2D world
    }
}

// Initialize (or re–initialize) the physics world with gravity gx, gy
void PhysicsWorld::init(float gx, float gy) {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);       // Destroy existing world
        BodyFactory::clearBodies();     // Remove all created bodies
    }
    b2WorldDef wdef = b2DefaultWorldDef();
    wdef.gravity = (b2Vec2){ gx, gy };  // Set gravity vector in world definition
    worldId_ = b2CreateWorld(&wdef);    // Create new Box2D world and store its ID
}

// Step the simulation forward by dt seconds
void PhysicsWorld::step(float dt) {
    if (B2_IS_NON_NULL(worldId_)) {
        b2World_Step(worldId_, dt, 8);  // 8 velocity/position sub-steps for stability
    }
}

// Expose internal world ID
b2WorldId PhysicsWorld::getWorldId() const {
    return worldId_;                    // Return the Box2D world handle
}

// Add a static horizontal ground segment at y, of given length and material properties
void PhysicsWorld::addGround(float y, float length, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;   // Do nothing if world isn't initialized

    groundY_ = y;     // Record for spawn checks
    b2BodyDef bd = b2DefaultBodyDef();  // Default static body definition
    bd.type = b2_staticBody;            // Mark as static so it does not move
    bd.position = (b2Vec2){ 0.0f, 0.0f }; // Body origin at world origin
    b2BodyId ground = b2CreateBody(worldId_, &bd);

    b2Segment seg{};                     // Define a line segment shape
    seg.point1 = (b2Vec2){ -length * 0.5f, y }; // Left end
    seg.point2 = (b2Vec2){  length * 0.5f, y }; // Right end

    b2ShapeDef sd = b2DefaultShapeDef(); // Default shape definition
    sd.material.restitution = restitution; // Bounciness when bodies collide
    sd.material.friction    = friction;    // Friction resisting sliding

    b2CreateSegmentShape(ground, &sd, &seg); // Attach shape to ground body
}

// Add a static horizontal roof segment, same as ground but at top
void PhysicsWorld::addRoof(float y, float length, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    roofY_ = y;     // Record for spawn checks
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ 0.0f, 0.0f };
    b2BodyId roof = b2CreateBody(worldId_, &bd);

    b2Segment seg{};
    seg.point1 = (b2Vec2){ -length * 0.5f, y };
    seg.point2 = (b2Vec2){  length * 0.5f, y };

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(roof, &sd, &seg);
}

// Add a static vertical left wall at x from bottom (0) to height
void PhysicsWorld::addLeftWall(float x, float height, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    leftX_ = x;    // Record for spawn checks
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ 0.0f, 0.0f };
    b2BodyId wall = b2CreateBody(worldId_, &bd);

    b2Segment seg{};
    seg.point1 = (b2Vec2){ x, 0.0f };     // Bottom point
    seg.point2 = (b2Vec2){ x, height };   // Top point

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution; // Bounce factor when dynamic bodies hit
    sd.material.friction    = friction;    // Friction along wall

    b2CreateSegmentShape(wall, &sd, &seg); // Attach the wall shape
    // When a dynamic body (e.g., box) collides:
    //  - Box2D detects contact with this static segment
    //  - It computes collision impulse based on relative velocity, restitution, friction
    //  - Wall stays fixed, box velocity is updated (bounced / slowed) accordingly
}

// Add a static vertical right wall (mirror of left)
void PhysicsWorld::addRightWall(float x, float height, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    rightX_ = x;     // Record for spawn checks
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ 0.0f, 0.0f };
    b2BodyId wall = b2CreateBody(worldId_, &bd);

    b2Segment seg{};
    seg.point1 = (b2Vec2){ x, 0.0f };
    seg.point2 = (b2Vec2){ x, height };

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(wall, &sd, &seg);
    // Identical collision response as left wall, just on opposite side
}

// Destroy the world and reset ID
void PhysicsWorld::destroy() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);       // Tear down Box2D world
        worldId_ = b2_nullWorldId;       // Mark as uninitialized
    }
}
float PhysicsWorld::getGroundY() const { return groundY_; }
float PhysicsWorld::getRoof()    const { return roofY_;   }
float PhysicsWorld::getLeftX()   const { return leftX_;   }
float PhysicsWorld::getRightX()  const { return rightX_;  }