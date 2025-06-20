// src/NativeBridge.cpp

#include "NativeBridge.h"
#include "PhysicsWorld.h"
#include "BodyFactory.h"
#include "Extras.h"
#include <box2d/box2d.h>

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_initWorld(
        JNIEnv*, jobject, jfloat gx, jfloat gy)
{
    PhysicsWorld::instance().init(gx, gy);
    PhysicsWorld::instance().addGround(0.0f, 200.0f, 0.0f, 0.5f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepWorld(
        JNIEnv*, jobject, jfloat dt)
{
    PhysicsWorld::instance().step(dt);
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyX(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    return B2_IS_NULL(id) ? 0.0f : b2Body_GetPosition(id).x;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyY(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    return B2_IS_NULL(id) ? 0.0f : b2Body_GetPosition(id).y;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyAngle(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    if (B2_IS_NULL(id)) return 0.0f;
    return b2Rot_GetAngle(b2Body_GetRotation(id));
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelX(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    return B2_IS_NULL(id) ? 0.0f : b2Body_GetLinearVelocity(id).x;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelY(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    return B2_IS_NULL(id) ? 0.0f : b2Body_GetLinearVelocity(id).y;
}

// BodyFactory controls
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyBody(
        JNIEnv*, jobject, jint idx)
{
    BodyFactory::destroyBody(idx);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_replaceBody(
        JNIEnv*, jobject, jint idx, jfloat x, jfloat y)
{
    BodyFactory::replaceBody(idx, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setBouncing(
        JNIEnv*, jobject, jint idx, jboolean en)
{
    BodyFactory::setBouncing(idx, en);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setVelocity(
        JNIEnv*, jobject, jint idx, jfloat vx, jfloat vy)
{
    BodyFactory::setVelocity(idx, vx, vy);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setAcceleration(
        JNIEnv*, jobject, jint idx, jfloat ax, jfloat ay)
{
    BodyFactory::setAcceleration(idx, ax, ay);
}

// World boundary builders
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addGround(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat re, jfloat f)
{
    PhysicsWorld::instance().addGround(y, length, re, f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRoof(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat re, jfloat f)
{
    PhysicsWorld::instance().addRoof(y, length, re, f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addLeftWall(
        JNIEnv*, jobject, jfloat x, jfloat h, jfloat re, jfloat f)
{
    PhysicsWorld::instance().addLeftWall(x, h, re, f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRightWall(
        JNIEnv*, jobject, jfloat x, jfloat h, jfloat re, jfloat f)
{
    PhysicsWorld::instance().addRightWall(x, h, re, f);
}

// World teardown & gravity
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyWorld(
        JNIEnv*, jobject)
{
    PhysicsWorld::instance().destroy();
    BodyFactory::clearBodies();
}

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setGravity(
        JNIEnv*, jobject, jfloat gx, jfloat gy)
{
    b2World_SetGravity(PhysicsWorld::instance().getWorldId(), { gx, gy });
}

// Extras dynamic/static creation
extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicSource(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateDynamicSource(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticSource(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateStaticSource(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicTarget(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateDynamicTarget(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticTarget(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateStaticTarget(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicObstacle(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateDynamicObstacle(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticObstacle(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution)
{
    return Extras_CreateStaticObstacle(
            static_cast<ShapeType>(shapeType),
            x, y, a, b,
            density, friction, restitution);
}

// Collision processing & queries
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_processCollisions(
        JNIEnv*, jobject)
{
    Extras_ProcessCollisions();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_isBodyAlive(
        JNIEnv*, jobject, jint idx)
{
    return BodyFactory::isBodyAlive(idx) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getLeftX(
        JNIEnv*, jobject)
{
    return PhysicsWorld::instance().getLeftX();
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getRightX(
        JNIEnv*, jobject)
{
    return PhysicsWorld::instance().getRightX();
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getGroundY(
        JNIEnv*, jobject)
{
    return PhysicsWorld::instance().getGroundY();
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getRoofY(
        JNIEnv*, jobject)
{
    return PhysicsWorld::instance().getRoof();
}
