// include/NativeBridge.h

#ifndef NATIVE_BRIDGE_H
#define NATIVE_BRIDGE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_initWorld(
        JNIEnv*, jobject, jfloat gx, jfloat gy);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_stepWorld(
        JNIEnv*, jobject, jfloat dt);

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

JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createCircle(
        JNIEnv*, jobject, jfloat x, jfloat y, jfloat r,
        jfloat d, jfloat f, jfloat re);

JNIEXPORT jint JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_createBox(
        JNIEnv*, jobject, jfloat x, jfloat y, jfloat w, jfloat h,
        jfloat d, jfloat f, jfloat re);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyBody(
        JNIEnv*, jobject, jint idx);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_replaceBody(
        JNIEnv*, jobject, jint idx, jfloat x, jfloat y);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setBouncing(
        JNIEnv*, jobject, jint idx, jboolean en);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setVelocity(
        JNIEnv*, jobject, jint idx, jfloat vx, jfloat vy);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setAcceleration(
        JNIEnv*, jobject, jint idx, jfloat ax, jfloat ay);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addGround(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat re, jfloat f);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRoof(
        JNIEnv*, jobject, jfloat y, jfloat length, jfloat re, jfloat f);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addLeftWall(
        JNIEnv*, jobject, jfloat x, jfloat h, jfloat re, jfloat f);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_addRightWall(
        JNIEnv*, jobject, jfloat x, jfloat h, jfloat re, jfloat f);
JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_destroyWorld(
        JNIEnv*, jobject);

JNIEXPORT void JNICALL
Java_com_aviadkorakin_demonstrate_12d_1physics_Box2DEngineNativeBridge_setGravity(
        JNIEnv*, jobject, jfloat gx, jfloat gy);
#ifdef __cplusplus
}
#endif

#endif // NATIVE_BRIDGE_H