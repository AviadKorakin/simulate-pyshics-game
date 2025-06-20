// Extras.cpp
#include "Extras.h"
#include "PhysicsWorld.h"
#include <cstdlib>
#include <box2d/box2d.h>

// Dynamic array for entity types
static EntityType* g_types = nullptr;
static int         g_capacity = 0;

// Ensure capacity
static void ensureCapacity(int idx) {
    if (idx < g_capacity) return;
    int newCap = g_capacity ? g_capacity : 4;
    while (idx >= newCap) newCap *= 2;
    g_types = (EntityType*)realloc(g_types, sizeof(EntityType) * newCap);
    for (int i = g_capacity; i < newCap; ++i) g_types[i] = OBSTACLE;
    g_capacity = newCap;
}

// Register entity type
void Extras_RegisterEntity(int idx, EntityType type) {
    if (idx < 0) return;
    ensureCapacity(idx + 1);
    g_types[idx] = type;
}

// Lookup index by body ID
static int lookupIndex(b2BodyId id) {
    size_t count = BodyFactory::getBodyCount();
    for (size_t ui = 0; ui < count; ++ui) {
        if (B2_ID_EQUALS(BodyFactory::getBodyId(static_cast<int>(ui)), id)) {
            return static_cast<int>(ui);
        }
    }
    return -1;
}

// Internal: dynamic creation for circle/box
static int createEntityDynamic(EntityType type,
                               ShapeType shape,
                               float x, float y,
                               float a, float b,
                               float density,
                               float friction,
                               float restitution)
{
    int idx = -1;
    switch (shape) {
        case SHAPE_CIRCLE:
            idx = BodyFactory::createDynamicBodyRound(x, y, a, density, friction, restitution);
            break;
        case SHAPE_BOX:
            idx = BodyFactory::createDynamicBodySquare(x, y, a, b, density, friction, restitution);
            break;
        default:
            return -1;
    }
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, type);
    // Enable contact events
    b2BodyId body = BodyFactory::getBodyId(idx);
    int shapeCount = b2Body_GetShapeCount(body);
    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);
    for (int si = 0; si < shapeCount; ++si) b2Shape_EnableContactEvents(shapes[si], true);
    return idx;
}

// Internal: static creation for circle/box
static int createEntityStatic(EntityType type,
                              ShapeType shape,
                              float x, float y,
                              float a, float b,
                              float density,
                              float friction,
                              float restitution)
{
    int idx = -1;
    switch (shape) {
        case SHAPE_CIRCLE:
            idx = BodyFactory::createStaticBodyRound(x, y, a, density, friction, restitution);
            break;
        case SHAPE_BOX:
            idx = BodyFactory::createStaticBodySquare(x, y, a, b, density, friction, restitution);
            break;
        default:
            return -1;
    }
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, type);
    // Enable contact events if needed
    b2BodyId body = BodyFactory::getBodyId(idx);
    int shapeCount = b2Body_GetShapeCount(body);
    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);
    for (int si = 0; si < shapeCount; ++si) b2Shape_EnableContactEvents(shapes[si], true);
    return idx;
}

// Public APIs: dynamic/static source, target, obstacle
int Extras_CreateDynamicSource(ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityDynamic(SOURCE, shape, x, y, a, b, density, friction, restitution);
}
int Extras_CreateStaticSource (ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityStatic (SOURCE, shape, x, y, a, b, density, friction, restitution);
}
int Extras_CreateDynamicTarget(ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityDynamic(TARGET, shape, x, y, a, b, density, friction, restitution);
}
int Extras_CreateStaticTarget (ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityStatic (TARGET, shape, x, y, a, b, density, friction, restitution);
}
int Extras_CreateDynamicObstacle(ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityDynamic(OBSTACLE, shape, x, y, a, b, density, friction, restitution);
}
int Extras_CreateStaticObstacle (ShapeType shape, float x, float y, float a, float b, float density, float friction, float restitution) {
    return createEntityStatic (OBSTACLE, shape, x, y, a, b, density, friction, restitution);
}

// Polygon-specific creation
int Extras_CreateDynamicSourcePolygon(const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, SOURCE);
    return idx;
}
int Extras_CreateStaticSourcePolygon (const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, SOURCE);
    return idx;
}
int Extras_CreateDynamicTargetPolygon(const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, TARGET);
    return idx;
}
int Extras_CreateStaticTargetPolygon (const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, TARGET);
    return idx;
}
int Extras_CreateDynamicObstaclePolygon(const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, OBSTACLE);
    return idx;
}
int Extras_CreateStaticObstaclePolygon (const std::vector<b2Vec2>& pts, float x, float y, float density, float friction, float restitution) {
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, density, friction, restitution);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, OBSTACLE);
    return idx;
}

// Process collisions
void Extras_ProcessCollisions() {
    b2WorldId wid = PhysicsWorld::instance().getWorldId();
    b2ContactEvents ev = b2World_GetContactEvents(wid);

    std::vector<int> toDestroy;
    toDestroy.reserve(ev.beginCount);

    for (int i = 0; i < ev.beginCount; ++i) {
        auto* e = &ev.beginEvents[i];
        b2BodyId bA = b2Shape_GetBody(e->shapeIdA);
        b2BodyId bB = b2Shape_GetBody(e->shapeIdB);
        int idxA = lookupIndex(bA);
        int idxB = lookupIndex(bB);
        if (idxA < 0 || idxB < 0) continue;

        if (g_types[idxA] == SOURCE && g_types[idxB] == TARGET) {
            toDestroy.push_back(idxB);
        } else if (g_types[idxB] == SOURCE && g_types[idxA] == TARGET) {
            toDestroy.push_back(idxA);
        }
    }

    for (int idx : toDestroy) {
        if (BodyFactory::isBodyAlive(idx)) {
            BodyFactory::destroyBody(idx);
            g_types[idx] = OBSTACLE;
        }
    }
}
