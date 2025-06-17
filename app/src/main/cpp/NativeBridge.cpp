// src/NativeBridge.cpp

#include "NativeBridge.h"
#include "PhysicsWorld.h"
#include "BodyFactory.h"
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
    if (B2_IS_NULL(id)) return 0.0f;
    return b2Body_GetPosition(id).x;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyY(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    if (B2_IS_NULL(id)) return 0.0f;
    return b2Body_GetPosition(id).y;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyAngle(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    if (B2_IS_NULL(id)) return 0.0f;
    b2Rot rot = b2Body_GetRotation(id);
    return b2Rot_GetAngle(rot);
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelX(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    if (B2_IS_NULL(id)) return 0.0f;
    return b2Body_GetLinearVelocity(id).x;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelY(
        JNIEnv*, jobject, jint idx)
{
    b2BodyId id = BodyFactory::getBodyId(idx);
    if (B2_IS_NULL(id)) return 0.0f;
    return b2Body_GetLinearVelocity(id).y;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createCircle(
        JNIEnv*, jobject, jfloat x, jfloat y, jfloat r,
        jfloat d, jfloat f, jfloat re)
{
    return BodyFactory::createCircle(x, y, r, d, f, re);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createBox(
        JNIEnv*, jobject, jfloat x, jfloat y, jfloat w, jfloat h,
        jfloat d, jfloat f, jfloat re)
{
    return BodyFactory::createBox(x, y, w, h, d, f, re);
}

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

extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyWorld(
        JNIEnv*, jobject)
{
    // Tear down Box2D world and clear all bodies
    PhysicsWorld::instance().destroy();     // you’ll add this
    BodyFactory::clearBodies();
}
extern "C" JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setGravity(
        JNIEnv*, jobject, jfloat gx, jfloat gy)
{
    b2World_SetGravity(PhysicsWorld::instance().getWorldId(), { gx, gy });
}