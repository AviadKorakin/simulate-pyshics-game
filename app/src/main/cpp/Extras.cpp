#include "Extras.h"
#include "PhysicsWorld.h"
#include <cstdlib>
#include <box2d/box2d.h>
#include <vector>
#include <algorithm>

// Dynamic array for entity types
static EntityType*       g_types       = nullptr;
static int               g_capacity    = 0;
static int               g_score       = 0;
static std::vector<int>  g_toDestroy;
static std::vector<bool> g_hadContact;

// Per-entity score values (default to 1)
static std::vector<int>  g_scoreValues;


// Ensure we have space for at least idx+1 entities
static void ensureCapacity(int idx) {
    if (idx < g_capacity) return;
    int newCap = g_capacity ? g_capacity : 4;
    while (idx >= newCap) newCap *= 2;

    // Resize the raw type array
    g_types = (EntityType*)realloc(g_types, sizeof(EntityType) * newCap);
    for (int i = g_capacity; i < newCap; ++i) {
        g_types[i] = OBSTACLE;      // default type
    }

    // Resize contact flags
    g_hadContact.resize(newCap, false);
    // Resize score values (default each to 1 point)
    g_scoreValues.resize(newCap, 1);

    g_capacity = newCap;
}

// Register entity type & (optionally) override its score value
void Extras_RegisterEntity(int idx, EntityType type) {
    if (idx < 0) return;
    ensureCapacity(idx);
    g_types[idx] = type;
}

// -- INTERNAL HELPERS --------------------------------------------------------

// Create dynamic (moving) entity, with optional scoreValue override
static int createEntityDynamic(EntityType type,
                               ShapeType shape,
                               float x, float y,
                               float a, float b,
                               float density,
                               float friction,
                               float restitution,
                               int scoreValue = 1)
{
    int idx = -1;
    switch (shape) {
        case SHAPE_CIRCLE:
            idx = BodyFactory::createDynamicBodyRound(
                    x, y, a, density, friction, restitution);
            break;
        case SHAPE_BOX:
            idx = BodyFactory::createDynamicBodySquare(
                    x, y, a, b, density, friction, restitution);
            break;
        default:
            return -1;
    }
    if (idx < 0) return -1;

    // Register and set per-entity score
    ensureCapacity(idx);
    g_types[idx]       = type;
    g_scoreValues[idx] = scoreValue;

    // Enable contact events
    b2BodyId body    = BodyFactory::getBodyId(idx);
    int shapeCount  = b2Body_GetShapeCount(body);
    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);
    for (int si = 0; si < shapeCount; ++si) {
        b2Shape_EnableContactEvents(shapes[si], true);
    }
    return idx;
}

// Create static (immovable) entity, with optional scoreValue
static int createEntityStatic(EntityType type,
                              ShapeType shape,
                              float x, float y,
                              float a, float b,
                              float density,
                              float friction,
                              float restitution,
                              int scoreValue = 1)
{
    int idx = -1;
    switch (shape) {
        case SHAPE_CIRCLE:
            idx = BodyFactory::createStaticBodyRound(
                    x, y, a, density, friction, restitution);
            break;
        case SHAPE_BOX:
            idx = BodyFactory::createStaticBodySquare(
                    x, y, a, b, density, friction, restitution);
            break;
        default:
            return -1;
    }
    if (idx < 0) return -1;

    // Register and set per-entity score
    ensureCapacity(idx);
    g_types[idx]       = type;
    g_scoreValues[idx] = scoreValue;

    // Enable contact events if desired
    b2BodyId body    = BodyFactory::getBodyId(idx);
    int shapeCount  = b2Body_GetShapeCount(body);
    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);
    for (int si = 0; si < shapeCount; ++si) {
        b2Shape_EnableContactEvents(shapes[si], true);
    }
    return idx;
}

// -- PUBLIC FACTORY FUNCTIONS ------------------------------------------------

int Extras_CreateDynamicSource(ShapeType s, float x, float y,
                               float a, float b,
                               float d, float f, float r)
{
    int idx = createEntityDynamic(SOURCE, s, x, y, a, b, d, f, r, /*score*/0);
        if (idx >= 0) {
                // enable CCD on the Box2D body
                        b2BodyId body = BodyFactory::getBodyId(idx);
                if (!B2_IS_NULL(body)) {
                        b2Body_SetBullet(body, true);
                    }
            }
       return idx;
}

int Extras_CreateStaticSource(ShapeType s, float x, float y,
                              float a, float b,
                              float d, float f, float r)
{
    return createEntityStatic(SOURCE, s, x, y, a, b, d, f, r, /*score*/0);
}

int Extras_CreateDynamicTarget(ShapeType s, float x, float y,
                               float a, float b,
                               float d, float f, float r,
                               int scoreValue)
{
    return createEntityDynamic(TARGET, s, x, y, a, b, d, f, r, scoreValue);
}

int Extras_CreateStaticTarget(ShapeType s, float x, float y,
                              float a, float b,
                              float d, float f, float r,
                              int scoreValue)
{
    return createEntityStatic(TARGET, s, x, y, a, b, d, f, r, scoreValue);
}

// Obstacles keep default score=0
int Extras_CreateDynamicObstacle(ShapeType s, float x, float y,
                                 float a, float b,
                                 float d, float f, float r)
{
    return createEntityDynamic(OBSTACLE, s, x, y, a, b, d, f, r, /*0*/0);
}

int Extras_CreateStaticObstacle(ShapeType s, float x, float y,
                                float a, float b,
                                float d, float f, float r)
{
    return createEntityStatic(STATIC_OBSTACLE, s, x, y, a, b, d, f, r, /*0*/0);
}

// Polygon overloads
int Extras_CreateDynamicSourcePolygon(const std::vector<b2Vec2>& pts,
                                      float x, float y,
                                      float d, float f, float r)
{
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, SOURCE);
    g_scoreValues[idx] = 0;
    return idx;
}

int Extras_CreateStaticSourcePolygon(const std::vector<b2Vec2>& pts,
                                     float x, float y,
                                     float d, float f, float r)
{
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, SOURCE);
    g_scoreValues[idx] = 0;
    return idx;
}

int Extras_CreateDynamicTargetPolygon(const std::vector<b2Vec2>& pts,
                                      float x, float y,
                                      float d, float f, float r,
                                      int scoreValue)
{
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, TARGET);
    g_scoreValues[idx] = scoreValue;
    return idx;
}

int Extras_CreateStaticTargetPolygon(const std::vector<b2Vec2>& pts,
                                     float x, float y,
                                     float d, float f, float r,
                                     int scoreValue)
{
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, TARGET);
    g_scoreValues[idx] = scoreValue;
    return idx;
}

int Extras_CreateDynamicObstaclePolygon(const std::vector<b2Vec2>& pts,
                                        float x, float y,
                                        float d, float f, float r)
{
    int idx = BodyFactory::createDynamicBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, OBSTACLE);
    g_scoreValues[idx] = 0;
    return idx;
}

int Extras_CreateStaticObstaclePolygon(const std::vector<b2Vec2>& pts,
                                       float x, float y,
                                       float d, float f, float r)
{
    int idx = BodyFactory::createStaticBodyPolygon(x, y, pts, d, f, r);
    if (idx < 0) return -1;
    Extras_RegisterEntity(idx, STATIC_OBSTACLE);
    g_scoreValues[idx] = 0;
    return idx;
}

// -- COLLISION & SCORING ----------------------------------------------------

void Extras_ProcessCollisions() {
    g_toDestroy.clear();

    // fetch collision events
    b2ContactEvents ev = b2World_GetContactEvents(
            PhysicsWorld::instance().getWorldId());

    for (int i = 0; i < ev.beginCount; ++i) {
        auto* e = &ev.beginEvents[i];
        int idxA = BodyFactory::lookupIndex(b2Shape_GetBody(e->shapeIdA));
        int idxB = BodyFactory::lookupIndex(b2Shape_GetBody(e->shapeIdB));
        if (idxA < 0 || idxB < 0) continue;

        EntityType typeA = g_types[idxA];
        EntityType typeB = g_types[idxB];

        // only care when SOURCE is involved
        if (typeA != SOURCE && typeB != SOURCE) continue;

        // if SOURCE hits TARGET → queue for destruction
        if ((typeA == SOURCE && typeB == TARGET) ||
            (typeB == SOURCE && typeA == TARGET)) {
            int victim = (typeA == TARGET ? idxA : idxB);
            g_toDestroy.push_back(victim);
        }

        // if SOURCE hits STATIC_OBSTACLE → mark that SOURCE had contact
        if ((typeA == SOURCE && typeB == STATIC_OBSTACLE) ||
            (typeB == SOURCE && typeA == STATIC_OBSTACLE)) {
            int sIdx = (typeA == SOURCE ? idxA : idxB);
            if ((size_t)sIdx < g_hadContact.size()) {
                g_hadContact[sIdx] = true;
            }
        }
    }

    // destroy and score targets/static obstacles
    for (int idx : g_toDestroy) {
        if (!BodyFactory::isBodyAlive(idx)) continue;
        BodyFactory::destroyBody(idx);
        g_types[idx] = OBSTACLE;  // downgraded

        // add the assigned score (defaults to 1 or 0)
        int pts = (idx < (int)g_scoreValues.size())
                  ? g_scoreValues[idx]
                  : 1;
        g_score += pts;
    }
}

bool Extras_HadContact(int idx) {
    if (idx < 0 || idx >= g_capacity) return false;
    return g_hadContact[idx];
}

void Extras_ClearContacts() {
    std::fill(g_hadContact.begin(), g_hadContact.end(), false);
}

int Extras_GetScore() {
    return g_score;
}

void Extras_ResetScore() {
    g_score = 0;
}

const std::vector<int>& Extras_GetLastDestroyed() {
    return g_toDestroy;
}