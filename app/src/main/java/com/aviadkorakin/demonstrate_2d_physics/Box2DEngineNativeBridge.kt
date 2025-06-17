package com.aviadkorakin.demonstrate_2d_physics

object Box2DEngineNativeBridge {
    init {
        // Load the native library (matches Android NDK sample)  [oai_citation:0‡developer.android.com](https://developer.android.com/ndk/samples/sample_hellojni?utm_source=chatgpt.com)
        System.loadLibrary("demonstrate_2d_physics")
    }

    /** Initialize world with custom gravity (gx, gy). */
    external fun initWorld(gx: Float, gy: Float)
    external fun destroyWorld()

    /** Step the simulation by dt seconds. */
    external fun stepWorld(dt: Float)

    /** Query body state */
    external fun getBodyX(idx: Int): Float
    external fun getBodyY(idx: Int): Float
    external fun getBodyAngle(idx: Int): Float
    external fun getBodyVelX(idx: Int): Float
    external fun getBodyVelY(idx: Int): Float

    /** Create shapes */
    external fun createCircle(
        x: Float, y: Float, radius: Float,
        density: Float, friction: Float, restitution: Float
    ): Int

    external fun createBox(
        x: Float, y: Float, halfWidth: Float, halfHeight: Float,
        density: Float, friction: Float, restitution: Float
    ): Int

    /** Remove a body */
    external fun destroyBody(idx: Int)

    /** New control functions */
    external fun replaceBody(idx: Int, x: Float, y: Float)
    external fun setBouncing(idx: Int, enable: Boolean)
    external fun setVelocity(idx: Int, vx: Float, vy: Float)
    external fun setAcceleration(idx: Int, ax: Float, ay: Float)
    external fun setGravity(gx: Float,gy: Float)

    /** World bounds */
    external fun addGround(y: Float, length: Float, restitution: Float, friction: Float)
    external fun addRoof(y: Float, length: Float, restitution: Float, friction: Float)
    external fun addLeftWall(x: Float, height: Float, restitution: Float, friction: Float)
    external fun addRightWall(x: Float, height: Float, restitution: Float, friction: Float)
}