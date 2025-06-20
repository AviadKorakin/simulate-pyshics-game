#include "BodyFactory.h"
#include "PhysicsWorld.h"
#include <box2d/box2d.h>

// Static storage for all created body IDs
std::vector<b2BodyId> BodyFactory::bodies_;

// Clears all stored bodies without destroying them in the world
void BodyFactory::clearBodies() {
    bodies_.clear();
}

// Retrieves the Box2D body ID at the given index, or b2_nullBodyId if out of range
b2BodyId BodyFactory::getBodyId(int idx) {
    if (idx < 0 || idx >= (int)bodies_.size()) {
        return b2_nullBodyId;    // Index invalid → return null ID
    }
    return bodies_[idx];       // Return the stored body ID
}
void BodyFactory::setBodyLocation(int idx, float x, float y) {
    // Teleport body: override position, keep rotation
    replaceBody(idx, x, y);
}

// Creates a dynamic circle body and returns its index in the bodies_ vector
int BodyFactory::createCircle(float x, float y, float radius,
                              float density, float friction, float restitution) {

    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY()    // below ground
        || y > world.getRoof()       // above roof (if desired)
        || x < world.getLeftX()      // left of left wall
        || x > world.getRightX()     // right of right wall
            ) {
        return -1;  // reject spawn
    }

    // 1) Define a dynamic body at position (x, y)
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(PhysicsWorld::instance().getWorldId(), &bd);

    // 2) Configure circle shape geometry
    b2Circle circle{};
    circle.radius = radius;

    // 3) Set material properties for the shape
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    // 4) Create the circle shape on the body
    b2CreateCircleShape(body, &sd, &circle);

    // 5) Store body ID and return its index
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

// Creates a dynamic box (polygon) body and returns its index
int BodyFactory::createBox(float x, float y, float halfWidth, float halfHeight,
                           float density, float friction, float restitution) {

    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY()    // below ground
        || y > world.getRoof()       // above roof (if desired)
        || x < world.getLeftX()      // left of left wall
        || x > world.getRightX()     // right of right wall
            ) {
        return -1;  // reject spawn
    }

    // 1) Define a dynamic body at (x, y)
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(PhysicsWorld::instance().getWorldId(), &bd);

    // 2) Create box polygon geometry
    b2Polygon poly = b2MakeBox(halfWidth, halfHeight);

    // 3) Set material properties
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    // 4) Attach the polygon shape to the body
    b2CreatePolygonShape(body, &sd, &poly);

    // 5) Store and return index
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}
int BodyFactory::createDynamicBodyRound(float x, float y, float radius,
                                        float density, float friction, float restitution) {
    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY() || y > world.getRoof() ||
        x < world.getLeftX()   || x > world.getRightX()) {
        return -1;
    }
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    b2Circle circle{};
    circle.radius = radius;

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    b2CreateCircleShape(body, &sd, &circle);
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

int BodyFactory::createStaticBodyRound(float x, float y, float radius,
                                       float density, float friction, float restitution) {
    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY() || y > world.getRoof() ||
        x < world.getLeftX()   || x > world.getRightX()) {
        return -1;
    }
    // static bodies generally no bounds check
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    b2Circle circle{};
    circle.radius = radius;

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    b2CreateCircleShape(body, &sd, &circle);
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

int BodyFactory::createDynamicBodySquare(float x, float y, float halfWidth, float halfHeight,
                                         float density, float friction, float restitution) {
    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY() || y > world.getRoof() ||
        x < world.getLeftX()   || x > world.getRightX()) {
        return -1;
    }
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    b2Polygon poly = b2MakeBox(halfWidth, halfHeight);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    b2CreatePolygonShape(body, &sd, &poly);
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

int BodyFactory::createStaticBodySquare(float x, float y, float halfWidth, float halfHeight,
                                        float density, float friction, float restitution) {

    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY() || y > world.getRoof() ||
        x < world.getLeftX()   || x > world.getRightX()) {
        return -1;
    }
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    b2Polygon poly = b2MakeBox(halfWidth, halfHeight);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    b2CreatePolygonShape(body, &sd, &poly);
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

int BodyFactory::createDynamicBodyPolygon(
        float x, float y,
        const std::vector<b2Vec2>& points,
        float density, float friction, float restitution)
{
    auto& world = PhysicsWorld::instance();
    // bounds check...

    // 1) Make a dynamic body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = { x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    // 2) Compute convex hull from input points
    b2Hull hull = b2ComputeHull(points.data(), int(points.size()));

    // 3) Build the polygon shape (no rounding)
    b2Polygon poly = b2MakePolygon(&hull, /*radius=*/0.0f);

    // 4) Define material properties
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    // 5) Attach shape to body
    b2CreatePolygonShape(body, &sd, &poly);

    // 6) Store and return
    bodies_.push_back(body);
    return int(bodies_.size() - 1);
}

int BodyFactory::createStaticBodyPolygon(    float x, float y,
                                             const std::vector<b2Vec2>& points,
                                             float density, float friction, float restitution)
{
    auto& world = PhysicsWorld::instance();
    if (y < world.getGroundY() || y > world.getRoof() ||
        x < world.getLeftX()   || x > world.getRightX()) {
        return -1;
    }

    // 1) Make a dynamic body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = { x, y };
    b2BodyId body = b2CreateBody(world.getWorldId(), &bd);

    // 2) Compute convex hull from input points
    b2Hull hull = b2ComputeHull(points.data(), int(points.size()));

    // 3) Build the polygon shape (no rounding)
    b2Polygon poly = b2MakePolygon(&hull, /*radius=*/0.0f);

    // 4) Define material properties
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = density;
    sd.material.friction = friction;
    sd.material.restitution = restitution;

    // 5) Attach shape to body
    b2CreatePolygonShape(body, &sd, &poly);

    // 6) Store and return
    bodies_.push_back(body);
    return int(bodies_.size() - 1);
}


// Destroys the body at index idx and removes it from storage
void BodyFactory::destroyBody(int idx) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) return;   // nothing to do

    // 1) Remove the body (and all its shapes) from the Box2D world
    b2DestroyBody(body);

    // 2) Mark the slot dead so our indices stay valid
    bodies_[idx] = b2_nullBodyId;
}

// Teleports an existing body to a new (x, y) while preserving its rotation
void BodyFactory::replaceBody(int idx, float x, float y) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) return;
    b2Rot rot = b2Body_GetRotation(body);
    b2Body_SetTransform(body, (b2Vec2){ x, y }, rot);
}

// Toggles the restitution (bounciness) on all shapes of a body
void BodyFactory::setBouncing(int idx, bool enable) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) return;

    // 1) Query shape count
    int shapeCount = b2Body_GetShapeCount(body);
    if (shapeCount <= 0) return;

    // 2) Collect all shape IDs attached to this body
    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);

    // 3) Set restitution on each shape: 1.0 for full bounce, 0.0 for no bounce
    float rest = enable ? 1.0f : 0.0f;
    for (int i = 0; i < shapeCount; ++i) {
        b2Shape_SetRestitution(shapes[i], rest);
    }
}

// Directly sets a body's linear velocity (in m/s)
void BodyFactory::setVelocity(int idx, float vx, float vy) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) return;
    b2Body_SetLinearVelocity(body, (b2Vec2){ vx, vy });
}

// Applies a continuous force to simulate an acceleration (F = m * a)
void BodyFactory::setAcceleration(int idx, float ax, float ay) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) return;


    // Compute force = mass * acceleration
    float mass = b2Body_GetMass(body);
    b2Vec2 force = { ax * mass, ay * mass };

    // Apply at center, wake body if sleeping
    b2Body_ApplyForceToCenter(body, force, true);
}
void BodyFactory::applyLaunchImpulse(int idx, float lastX, float lastY, float newX, float newY, float dt) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body) || dt <= 0) return;
    // Compute velocity from drag delta
    float vx = (newX - lastX) / dt;
    float vy = (newY - lastY) / dt;
    b2Body_SetLinearVelocity(body, { vx, vy });
}
bool BodyFactory::isBodyAlive(int idx) {
    b2BodyId body = getBodyId(idx);
    if (B2_IS_NULL(body)) {
        return false;  // index invalid or already destroyed
    }

    // Get current world position
    b2Vec2 pos = b2Body_GetPosition(body);

    // Fetch exact world bounds
    float left   = PhysicsWorld::instance().getLeftX();
    float right  = PhysicsWorld::instance().getRightX();
    float bottom = PhysicsWorld::instance().getGroundY();
    float top    = PhysicsWorld::instance().getRoof();

    bool clamped = false;
    float cx = pos.x, cy = pos.y;

    if (pos.x < left)   { cx = left;   clamped = true; }
    if (pos.x > right)  { cx = right;  clamped = true; }
    if (pos.y < bottom) { cy = bottom; clamped = true; }
    if (pos.y > top)    { cy = top;    clamped = true; }

    if (clamped) {
        // teleport back inside, preserving rotation
        b2Rot rot = b2Body_GetRotation(body);
        b2Body_SetTransform(body, (b2Vec2){cx, cy}, rot);
    }

    return true;
}

size_t BodyFactory::getBodyCount() {
    return bodies_.size();
}