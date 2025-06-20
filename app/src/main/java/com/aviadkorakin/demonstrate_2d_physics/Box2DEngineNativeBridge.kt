package com.aviadkorakin.demonstrate_2d_physics

object Box2DEngineNativeBridge {
    init {
        System.loadLibrary("demonstrate_2d_physics")
    }

    /** Initialize / destroy world */
    external fun initWorld(gx: Float, gy: Float)
    external fun destroyWorld()

    /** Simulation step */
    external fun stepWorld(dt: Float)

    /** Body queries */
    external fun getBodyX(idx: Int): Float
    external fun getBodyY(idx: Int): Float
    external fun getBodyAngle(idx: Int): Float
    external fun getBodyVelX(idx: Int): Float
    external fun getBodyVelY(idx: Int): Float
    external fun isBodyAlive(idx: Int): Boolean

    /** Basic creation */
    external fun createCircle(
        x: Float, y: Float, radius: Float,
        density: Float, friction: Float, restitution: Float
    ): Int
    external fun createBox(
        x: Float, y: Float, halfWidth: Float, halfHeight: Float,
        density: Float, friction: Float, restitution: Float
    ): Int

    /** Removal */
    external fun destroyBody(idx: Int)

    /** Property setters */
    external fun replaceBody(idx: Int, x: Float, y: Float)
    external fun setBouncing(idx: Int, enable: Boolean)
    external fun setVelocity(idx: Int, vx: Float, vy: Float)
    external fun setAcceleration(idx: Int, ax: Float, ay: Float)
    external fun setGravity(gx: Float, gy: Float)

    /** World boundaries */
    external fun addGround(y: Float, length: Float, restitution: Float, friction: Float)
    external fun addRoof(y: Float, length: Float, restitution: Float, friction: Float)
    external fun addLeftWall(x: Float, height: Float, restitution: Float, friction: Float)
    external fun addRightWall(x: Float, height: Float, restitution: Float, friction: Float)

    // ── Extras: source/target/obstacle support ────────────────────────────────

    /**
     * Create a dynamic SOURCE body.
     */
    external fun createDynamicSource(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /**
     * Create a static SOURCE body.
     */
    external fun createStaticSource(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /**
     * Create a dynamic TARGET body.
     */
    external fun createDynamicTarget(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /**
     * Create a static TARGET body.
     */
    external fun createStaticTarget(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /**
     * Create a dynamic OBSTACLE body.
     */
    external fun createDynamicObstacle(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /**
     * Create a static OBSTACLE body.
     */
    external fun createStaticObstacle(
        shapeType: Int,
        x: Float, y: Float,
        a: Float, b: Float,
        density: Float,
        friction: Float,
        restitution: Float
    ): Int

    /** Poll contacts and auto-destroy any TARGET hit by a SOURCE */
    external fun processCollisions()

    /** World bounds queries */
    external fun getLeftX(): Float
    external fun getRightX(): Float
    external fun getGroundY(): Float
    external fun getRoofY(): Float
}
