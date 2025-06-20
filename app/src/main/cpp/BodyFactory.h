#ifndef BODYFACTORY_H
#define BODYFACTORY_H

#include <vector>
#include <box2d/box2d.h>

class BodyFactory {
public:
    /// Clear all stored body IDs
    static void clearBodies();

    /// Retrieve a body handle or null
    static b2BodyId getBodyId(int idx);

    /// Number of bodies stored
    static size_t getBodyCount();

    /// Create shapes
    static int createCircle(float x, float y, float radius,
                            float density, float friction, float restitution);
    static int createBox(float x, float y,
                         float halfWidth, float halfHeight,
                         float density, float friction, float restitution);

    // Dynamic & Static Round (Circle)
    static int createDynamicBodyRound(float x, float y, float radius,
                                      float density, float friction, float restitution);
    static int createStaticBodyRound(float x, float y, float radius,
                                     float density, float friction, float restitution);

    // Dynamic & Static Square (Box)
    static int createDynamicBodySquare(float x, float y, float halfWidth, float halfHeight,
                                       float density, float friction, float restitution);
    static int createStaticBodySquare(float x, float y, float halfWidth, float halfHeight,
                                      float density, float friction, float restitution);

    // Dynamic & Static Polygon
    static int createDynamicBodyPolygon(float x, float y, const std::vector<b2Vec2>& points,
                                 float density, float friction, float restitution);
    static int createStaticBodyPolygon(float x, float y, const std::vector<b2Vec2>& points,
                                float density, float friction, float restitution);


    /// Manage bodies
    static void destroyBody(int idx);
    static void replaceBody(int idx, float x, float y);
    static void setBouncing(int idx, bool enable);
    static void setVelocity(int idx, float vx, float vy);
    static void setAcceleration(int idx, float ax, float ay);

    /// New: directly set body location (teleport)
    static void setBodyLocation(int idx, float x, float y);

    /// New: apply impulse based on drag launch (px,py) -> (dx,dy), dt
    static void applyLaunchImpulse(int idx, float lastX, float lastY, float newX, float newY, float dt);

    static bool isBodyAlive(int idx);


    static std::vector<b2BodyId> bodies_;
};

#endif // BODYFACTORY_H