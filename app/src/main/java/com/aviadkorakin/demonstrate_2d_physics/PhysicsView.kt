package com.aviadkorakin.demonstrate_2d_physics

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.os.Build
import android.os.Handler
import android.os.HandlerThread
import android.util.AttributeSet
import android.view.Choreographer
import android.view.Display
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.WindowManager
import kotlin.math.roundToInt

/**
 * A custom SurfaceView that aligns physics stepping to the display VSYNC
 * by signaling a dedicated physics thread on each frame.
 */
class PhysicsView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback, Choreographer.FrameCallback {

    // Paint and bitmaps decoded once to avoid per-frame allocations
    private val paint = Paint(Paint.ANTI_ALIAS_FLAG)
    private val circleBmp = BitmapFactory.decodeResource(resources, R.drawable.ic_circle)
    private val boxBmp    = BitmapFactory.decodeResource(resources, R.drawable.ic_box)

    // List of (body index, bitmap) pairs for rendering
    private val objects = mutableListOf<Pair<Int, Bitmap>>()

    // Display sync and timing
    private val choreographer = Choreographer.getInstance() // VSYNC callback
    private var lastFrameTimeNanos = 0L                     // last VSYNC timestamp

    // Physics thread and handler
    private lateinit var physicsThread: HandlerThread
    private lateinit var physicsHandler: Handler

    // Scale and bounds
    private val pxPerMeter = 50f                            // physics → screen conversion

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        // --- Setup Box2D world once ---
        Box2DEngineNativeBridge.initWorld(0f, -9.8f)
        val w = width  / pxPerMeter
        val h = (height - 100f) / pxPerMeter
        Box2DEngineNativeBridge.addGround(   0f,    w,   0f, 0.5f)
        Box2DEngineNativeBridge.addRoof(     h,    w,   0f, 0.5f)
        Box2DEngineNativeBridge.addLeftWall(-w/2,   h,   0f, 0.5f)
        Box2DEngineNativeBridge.addRightWall(w/2,    h,   0f, 0.5f)

        // --- Create and start physics handler thread ---
        physicsThread = HandlerThread("PhysicsThread").apply { start() }
        physicsHandler = Handler(physicsThread.looper)

        // --- Begin VSYNC-driven render & physics loop ---
        choreographer.postFrameCallback(this)
    }

    override fun doFrame(frameTimeNanos: Long) {
        // On first frame, initialize timestamp
        if (lastFrameTimeNanos == 0L) lastFrameTimeNanos = frameTimeNanos

        // Compute elapsed seconds since last VSYNC
        val dt = (frameTimeNanos - lastFrameTimeNanos) / 1_000_000_000f
        lastFrameTimeNanos = frameTimeNanos

        // Signal physics thread to step world using this VSYNC-driven dt
        physicsHandler.post {
            Box2DEngineNativeBridge.stepWorld(dt)
        }

        // Immediately render the latest physics state
        val canvas: Canvas? = holder.lockCanvas()
        canvas?.let {
            it.drawColor(Color.WHITE)
            synchronized(objects) {
                for ((idx, bmp) in objects) {
                    val x = Box2DEngineNativeBridge.getBodyX(idx) * pxPerMeter + width / 2f
                    val y = height - (Box2DEngineNativeBridge.getBodyY(idx) * pxPerMeter + 100f)
                    it.drawBitmap(bmp, x - bmp.width / 2f, y - bmp.height / 2f, paint)
                }
            }
            holder.unlockCanvasAndPost(it)
        }

        // Queue next VSYNC callback
        choreographer.postFrameCallback(this)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        // Clean up VSYNC and physics thread
        choreographer.removeFrameCallback(this)
        physicsThread.quitSafely()
        physicsThread.join()

        // Destroy physics world and clear objects
        Box2DEngineNativeBridge.destroyWorld()
        synchronized(objects) { objects.clear() }
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        // no-op
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (event.action == MotionEvent.ACTION_DOWN) {
            val tx = event.x
            val ty = event.y
            synchronized(objects) {
                // Hit test existing bodies
                val hit = objects.indexOfFirst { (idx, bmp) ->
                    val x = Box2DEngineNativeBridge.getBodyX(idx) * pxPerMeter + width / 2f
                    val y = height - (Box2DEngineNativeBridge.getBodyY(idx) * pxPerMeter + 100f)
                    tx in (x - bmp.width/2f)..(x + bmp.width/2f) &&
                            ty in (y - bmp.height/2f)..(y + bmp.height/2f)
                }
                if (hit >= 0) {
                    val (bodyIdx, _) = objects.removeAt(hit)
                    Box2DEngineNativeBridge.destroyBody(bodyIdx)
                    return true
                }
            }
            // Spawn new body and add to list
            val xM = (tx - width / 2f) / pxPerMeter
            val yM = (height - ty - 100f) / pxPerMeter
            val pair = if (event.buttonState and MotionEvent.BUTTON_SECONDARY != 0) {
                val id = Box2DEngineNativeBridge.createBox(xM, yM, 0.5f, 0.5f, 1f, 0.3f, 0.8f)
                Box2DEngineNativeBridge.setBouncing(id, true)
                id to boxBmp
            } else {
                val id = Box2DEngineNativeBridge.createCircle(xM, yM, 0.5f, 1f, 0.3f, 0.8f)
                Box2DEngineNativeBridge.setBouncing(id, true)
                id to circleBmp
            }
            synchronized(objects) { objects += pair }
            return true
        }
        return super.onTouchEvent(event)
    }
}
