#include "PhysicsWorld.h"
#include "BodyFactory.h"
#include "Extras.h"
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
void PhysicsWorld::stepPlusCollisons(float dt) {
    if (B2_IS_NON_NULL(worldId_)) {
        // 1) advance the physics
        b2World_Step(worldId_, dt, 8);

        Extras_ProcessCollisions();
    }
}

// Expose internal world ID
b2WorldId PhysicsWorld::getWorldId() const {
    return worldId_;                    // Return the Box2D world handle
}

// Add a static horizontal ground segment at y, of given length and material properties
void PhysicsWorld::addGround(float y, float length, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    groundY_ = y;
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = {0,0};
    b2BodyId ground = b2CreateBody(worldId_, &bd);


    b2Segment seg{};
    seg.point1 = { -length * 0.5f, y };
    seg.point2 = {  length * 0.5f, y };

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(ground, &sd, &seg);
}

void PhysicsWorld::addRoof(float y, float length, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    roofY_ = y;
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = {0,0};
    b2BodyId roof = b2CreateBody(worldId_, &bd);

    // register it as an OBSTACLE
    int idx = BodyFactory::lookupIndex(roof);
    Extras_RegisterEntity(idx, STATIC_OBSTACLE);

    b2Segment seg{};
    seg.point1 = { -length * 0.5f, y };
    seg.point2 = {  length * 0.5f, y };

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(roof, &sd, &seg);
}

void PhysicsWorld::addLeftWall(float x, float height, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    leftX_ = x;
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = {0,0};
    b2BodyId wall = b2CreateBody(worldId_, &bd);

    // register it as an OBSTACLE
    int idx = BodyFactory::lookupIndex(wall);
    Extras_RegisterEntity(idx, STATIC_OBSTACLE);

    b2Segment seg{};
    seg.point1 = { x, 0.0f };
    seg.point2 = { x, height };


    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(wall, &sd, &seg);
}

void PhysicsWorld::addRightWall(float x, float height, float restitution, float friction) {
    if (B2_IS_NULL(worldId_)) return;

    rightX_ = x;
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = {0,0};
    b2BodyId wall = b2CreateBody(worldId_, &bd);

    // register it as an OBSTACLE
    int idx = BodyFactory::lookupIndex(wall);
    Extras_RegisterEntity(idx, STATIC_OBSTACLE);

    b2Segment seg{};
    seg.point1 = { x, 0.0f };
    seg.point2 = { x, height };

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.material.restitution = restitution;
    sd.material.friction    = friction;

    b2CreateSegmentShape(wall, &sd, &seg);
}

// Destroy the world and reset ID
void PhysicsWorld::destroy() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);            // tear down the Box2D world
        worldId_ = b2_nullWorldId;           // mark it invalid

        BodyFactory::clearBodies();          // <-- wipe out all stored body IDs
    }
}
float PhysicsWorld::getGroundY() const { return groundY_; }
float PhysicsWorld::getRoof()    const { return roofY_;   }
float PhysicsWorld::getLeftX()   const { return leftX_;   }
float PhysicsWorld::getRightX()  const { return rightX_;  }