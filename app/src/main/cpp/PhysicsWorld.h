#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <box2d/box2d.h>

class PhysicsWorld {
public:
    /// Singleton access
    static PhysicsWorld& instance();


    /// Initialize or reset the world with gravity (gx, gy)
    void init(float gx, float gy);

    /// Advance the simulation by dt seconds
    void step(float dt);

    /// Expose the underlying C-API world handle
    [[nodiscard]] b2WorldId getWorldId() const;

    /// Add static boundaries
    void addGround(float y, float length, float restitution, float friction);
    void addRoof(float y, float length, float restitution, float friction);
    void addLeftWall(float x, float height, float restitution, float friction);
    void addRightWall(float x, float height, float restitution, float friction);

    [[nodiscard]] float getGroundY() const;
    [[nodiscard]] float getLeftX() const;
    [[nodiscard]] float getRightX() const;
    [[nodiscard]] float getRoof() const;

    /// Deleted so the singleton cannot be copied or assigned
    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld&) = delete;
    void destroy();

private:
    PhysicsWorld();
    ~PhysicsWorld();

    b2WorldId worldId_;

    float groundY_{ 0.0f };
    float roofY_{ 0.0f };
    float leftX_{ 0.0f };
    float rightX_{ 0.0f };
};

#endif // PHYSICSWORLD_H