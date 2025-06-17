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

    /// Create shapes
    static int createCircle(float x, float y, float radius,
                            float density, float friction, float restitution);
    static int createBox(float x, float y,
                         float halfWidth, float halfHeight,
                         float density, float friction, float restitution);

    /// Manage bodies
    static void destroyBody(int idx);
    static void replaceBody(int idx, float x, float y);
    static void setBouncing(int idx, bool enable);
    static void setVelocity(int idx, float vx, float vy);
    static void setAcceleration(int idx, float ax, float ay);

private:
    static std::vector<b2BodyId> bodies_;
};

#endif // BODYFACTORY_H