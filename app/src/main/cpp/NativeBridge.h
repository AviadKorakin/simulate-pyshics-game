// include/NativeBridge.h
#ifndef NATIVE_BRIDGE_H
#define NATIVE_BRIDGE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

// World initialization and stepping
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_initWorld(
        JNIEnv*, jobject, jfloat gx, jfloat gy);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepWorld(
        JNIEnv*, jobject, jfloat dt);

// Body query functions
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyX(
        JNIEnv*, jobject, jint idx);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyY(
        JNIEnv*, jobject, jint idx);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyAngle(
        JNIEnv*, jobject, jint idx);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelX(
        JNIEnv*, jobject, jint idx);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyVelY(
        JNIEnv*, jobject, jint idx);

// Body control functions
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyBody(
        JNIEnv*, jobject, jint idx);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_replaceBody(
        JNIEnv*, jobject, jint idx, jfloat x, jfloat y);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setBouncing(
        JNIEnv*, jobject, jint idx, jboolean enable);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setVelocity(
        JNIEnv*, jobject, jint idx, jfloat vx, jfloat vy);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setAcceleration(
        JNIEnv*, jobject, jint idx, jfloat ax, jfloat ay);

// World boundary builders
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addGround(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat restitution, jfloat friction);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRoof(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat restitution, jfloat friction);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addLeftWall(
        JNIEnv*, jobject, jfloat x, jfloat height, jfloat restitution, jfloat friction);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRightWall(
        JNIEnv*, jobject, jfloat x, jfloat height, jfloat restitution, jfloat friction);

// World teardown & gravity
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyWorld(
        JNIEnv*, jobject);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setGravity(
        JNIEnv*, jobject, jfloat gx, jfloat gy);

// Dynamic/Static creation for sources, targets, obstacles
JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicSource(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution);
JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticSource(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution);

JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicTarget(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution,jint scoreValue);
JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticTarget(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution,jint scoreValue);

JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createDynamicObstacle(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution);
JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createStaticObstacle(
        JNIEnv*, jobject, jint shapeType,
        jfloat x, jfloat y,
        jfloat a, jfloat b,
        jfloat density, jfloat friction, jfloat restitution);

// Collision processing & queries
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_processCollisions(
        JNIEnv*, jobject);
JNIEXPORT jboolean JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_isBodyAlive(
        JNIEnv*, jobject, jint idx);

// World bounds queries
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getLeftX(
        JNIEnv*, jobject);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getRightX(
        JNIEnv*, jobject);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getGroundY(
        JNIEnv*, jobject);
JNIEXPORT jfloat JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getRoofY(
        JNIEnv*, jobject);
JNIEXPORT jfloatArray JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getAllBodyPositions(
        JNIEnv* env, jobject );
JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getScore(JNIEnv *env,
                                                                                jobject );
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_resetScore(JNIEnv *env,
                                                                                  jobject );
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepWorldPlusCollisions( JNIEnv*, jobject, jfloat d);

JNIEXPORT jintArray JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepAndGetRemoved(
        JNIEnv* env, jobject, jfloat dt);
JNIEXPORT jfloatArray JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_getBodyPosition(
        JNIEnv* env, jobject /* self */, jint idx);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_teleportAndStop(
        JNIEnv* env, jobject /* this */,
jint idx, jfloat x, jfloat y);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setGravityScale(
        JNIEnv* /*env*/, jobject /*self*/,
        jint idx, jfloat scale);

JNIEXPORT jboolean JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_extrasHadContact(
        JNIEnv*, jobject, jint idx);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_extrasClearContacts(
        JNIEnv*, jobject);

#ifdef __cplusplus
}
#endif

#endif // NATIVE_BRIDGE_H
