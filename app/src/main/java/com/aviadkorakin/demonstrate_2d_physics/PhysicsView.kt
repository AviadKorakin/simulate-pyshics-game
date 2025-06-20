package com.aviadkorakin.demonstrate_2d_physics

import android.content.Context
import android.graphics.*
import android.os.Handler
import android.os.HandlerThread
import android.util.AttributeSet
import android.view.Choreographer
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.core.graphics.scale
import kotlin.math.roundToInt
import kotlin.random.Random

/**
 * A SurfaceView that drives a Box2D simulation on a separate thread,
 * renders synced to VSYNC, and lets you drag-and-launch a single "source" body.
 * On startup it spawns 10 circle targets, 10 box obstacles, and 1 circle source.
 */
class PhysicsView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback, Choreographer.FrameCallback {

    // Paints and resources
    private val paint = Paint(Paint.ANTI_ALIAS_FLAG)
    private val pillarPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.FILL
        color = Color.BLACK
    }
    private val targetBmp   = BitmapFactory.decodeResource(resources, R.drawable.ic_target)
    private val obstacleBmp = BitmapFactory.decodeResource(resources, R.drawable.ic_obstacle)
    private val sourceBmp   = BitmapFactory.decodeResource(resources, R.drawable.ic_source)
    private val flagBmp     = BitmapFactory.decodeResource(resources, R.drawable.ic_iran_flag)

    // Size constants
    private val pxPerMeter   = 50f
    private val pillarWidthPx  = 100
    private val pillarHeightPx get() = height / 3
    private val shelfWidthPx  = 250
    private val shelfHeightPx = 100
    private val flagWidthPx   = 120
    private val flagHeightPx  = 120
    private var shelfWorldCX = 0f
    private var shelfWorldCY = 0f

    // Background bitmap
    private var backgroundBmp: Bitmap? = null

    // Render items
    private data class RenderItem(val idx: Int, val bmp: Bitmap, val w: Int, val h: Int)
    private val objects = mutableListOf<RenderItem>()
    private var sourceIdx = -1

    // Physics loop
    private val choreographer = Choreographer.getInstance()
    private var lastFrameTimeNanos = 0L
    private lateinit var physicsThread: HandlerThread
    private lateinit var physicsHandler: Handler

    // Drag state
    private var isDragging = false
    private var dragLastX = 0f
    private var dragLastY = 0f
    private var dragPrevTime = 0L
    private var dragVelocityX = 0f
    private var dragVelocityY = 0f


    // Track static regions for collision prevention
    private data class WorldRect(val minX: Float, val minY: Float, val maxX: Float, val maxY: Float)
    private val staticWorldRegions = mutableListOf<WorldRect>()

    init { holder.addCallback(this) }

    // Helper: scale any bitmap
    private fun scaleBitmap(original: Bitmap, wPx: Int, hPx: Int): Bitmap =
        original.scale(wPx, hPx)

    // Public: set custom background
    fun setBackgroundImage(bitmap: Bitmap) { backgroundBmp = bitmap }

    override fun surfaceCreated(holder: SurfaceHolder) {
        loadAndPrepareBackground(R.drawable.background)
        initializeWorld()
        spawnInitialBodies()
        startPhysicsLoop()
    }

    private fun initializeWorld() {
        Box2DEngineNativeBridge.initWorld(0f, -9.8f)
        val w = width  / pxPerMeter
        val h = (height - 100f) / pxPerMeter
        Box2DEngineNativeBridge.addGround(   0f,    w, 0f, 0.5f)
        Box2DEngineNativeBridge.addRoof(     h,    w, 0f, 0.5f)
        Box2DEngineNativeBridge.addLeftWall(-w/2f,  h, 0f, 0.5f)
        Box2DEngineNativeBridge.addRightWall(w/2f,  h, 0f, 0.5f)

        // Pillar static body + record region
        val pW = pillarWidthPx / pxPerMeter
        val pH = pillarHeightPx / pxPerMeter
        val cX = -w/2f + pW/2f
        val cY = pH/2f
        Box2DEngineNativeBridge.createStaticObstacle(1, cX, cY, pW/2f, pH/2f, 0f, 0.5f, 0f)
        staticWorldRegions += WorldRect(cX - pW/2f, 0f, cX + pW/2f, pH)

        // Flag-pole static body + region
        val fW = (flagWidthPx / pxPerMeter) / 2f
        val fH = (flagHeightPx / pxPerMeter) / 2f
        val fCX = cX
        val fCY = cY + pH/2f + fH
        Box2DEngineNativeBridge.createStaticObstacle(1, fCX, fCY, fW, fH, 0f, 0.5f, 0f)
        staticWorldRegions += WorldRect(fCX - fW, pH, fCX + fW, pH + fH*2)

        // screen‐pixel coords
            val shelfTopPx = 200f
            val shelfBottomPx = shelfTopPx + shelfHeightPx
            // world dims
            val sW = shelfWidthPx / pxPerMeter
            val sH = shelfHeightPx / pxPerMeter
            // world‐X: right edge minus half‐width:
        // world‐center of shelf X
        shelfWorldCX = ((width - shelfWidthPx/2f) - width/2f) / pxPerMeter
        // world‐center of shelf Y
        shelfWorldCY = (height - (shelfTopPx + shelfBottomPx)/2f - 100f) / pxPerMeter

            // now create the physics obstacle
            Box2DEngineNativeBridge.createStaticObstacle(
                    1, shelfWorldCX, shelfWorldCY, sW/2f, sH/2f,
                    0f, 0.5f, 0f
                        )
        staticWorldRegions += WorldRect(
                            shelfWorldCX - sW/2f, shelfWorldCY - sH/2f,
                    shelfWorldCX + sW/2f, shelfWorldCY + sH/2f
        )
    }

    private fun spawnInitialBodies() {
        spawnTargets()
        spawnObstacles()
        spawnSource()
    }

    private fun spawnTargets() {
        val w = width  / pxPerMeter
        val h = (height - 100f) / pxPerMeter

        repeat(10) {
            val r = 0.8f
            var x: Float
            var y: Float

            // Keep sampling until the point is outside any static region
            do {
                x = Random.nextFloat() * w - w / 2f
                y = Random.nextFloat() * h
            } while (staticWorldRegions.any { rect ->
                    x in rect.minX..rect.maxX && y in rect.minY..rect.maxY
                })

            val id = Box2DEngineNativeBridge.createDynamicTarget(
                0, x, y, r, 0f,
                density   = 1f,
                friction  = 0.3f,
                restitution = 0.8f
            )
            val size = (r * 2 * pxPerMeter).roundToInt()
            val bmp  = scaleBitmap(targetBmp, size, size)
            objects += RenderItem(id, bmp, size, size)
        }
    }

    private fun spawnObstacles() {
        val w = width  / pxPerMeter
        val h = (height - 100f) / pxPerMeter

        repeat(10) {
            val halfW = 0.8f
            val halfH = 0.8f
            var x: Float
            var y: Float

            // Keep sampling until the point is outside any static region
            do {
                x = Random.nextFloat() * w - w / 2f
                y = Random.nextFloat() * h
            } while (staticWorldRegions.any { rect ->
                    x in rect.minX..rect.maxX && y in rect.minY..rect.maxY
                })

            val id = Box2DEngineNativeBridge.createDynamicObstacle(
                1, x, y, halfW, halfH,
                density     = 1f,
                friction    = 0.3f,
                restitution = 0.8f
            )
            val wPx = (halfW * 2 * pxPerMeter).roundToInt()
            val hPx = (halfH * 2 * pxPerMeter).roundToInt()
            val bmp = scaleBitmap(obstacleBmp, wPx, hPx)
            objects += RenderItem(id, bmp, wPx, hPx)
        }
    }

    private fun spawnSource() {
        val r = 2f
        // place source exactly one radius above the shelf
        val srcX = shelfWorldCX
        val srcY = shelfWorldCY + (shelfHeightPx / pxPerMeter) / 2f + r

        sourceIdx = Box2DEngineNativeBridge.createDynamicSource(
            0, srcX, srcY, r, 0f,
            density     = 2f,
            friction    = 0.2f,
            restitution = 0.5f
        )
        val size = (r * 2 * pxPerMeter).roundToInt()
        val bmp  = scaleBitmap(sourceBmp, size, size)
        objects += RenderItem(sourceIdx, bmp, size, size)
    }
    private fun startPhysicsLoop() {
        physicsThread = HandlerThread("PhysicsThread").apply { start() }
        physicsHandler = Handler(physicsThread.looper)
        choreographer.postFrameCallback(this)
    }

    override fun doFrame(frameTimeNanos: Long) {
        if (lastFrameTimeNanos == 0L) lastFrameTimeNanos = frameTimeNanos
        val dt = (frameTimeNanos - lastFrameTimeNanos) / 1_000_000_000f
        lastFrameTimeNanos = frameTimeNanos

        // Physics step
        physicsHandler.post {
            Box2DEngineNativeBridge.stepWorld(dt)
            Box2DEngineNativeBridge.processCollisions()
            synchronized(objects) { objects.removeAll { !Box2DEngineNativeBridge.isBodyAlive(it.idx) } }
        }

        // Rendering
        val canvas = holder.lockCanvas() ?: return
        // Background
        backgroundBmp?.let { canvas.drawBitmap(it, 0f, 0f, paint) } ?: canvas.drawColor(Color.WHITE)
        // Pillar + flag
        val left = 0f
        val right = left + pillarWidthPx
        val bottom = height - 100f
        val top = bottom - pillarHeightPx
        canvas.drawRect(left, top, right, bottom, pillarPaint)
        val fx = left + (pillarWidthPx - flagWidthPx) / 2f
        val fy = top - flagHeightPx
        canvas.drawBitmap(scaleBitmap(flagBmp, flagWidthPx, flagHeightPx), fx, fy, paint)

        // ── Draw the right-side shelf ─────────────────────────────────────────
           val shelfLeftPx   = width  - shelfWidthPx.toFloat()
           val shelfTopPx    = 200f
            val shelfRightPx  = width.toFloat()
            val shelfBottomPx = shelfTopPx + shelfHeightPx
            canvas.drawRect(
                    shelfLeftPx,
                    shelfTopPx,
                    shelfRightPx,
                   shelfBottomPx,
                    pillarPaint
                        )


        // Bodies
        synchronized(objects) {
            for (item in objects) {
                val px = Box2DEngineNativeBridge.getBodyX(item.idx) * pxPerMeter + width / 2f
                val py = height - (Box2DEngineNativeBridge.getBodyY(item.idx) * pxPerMeter + 100f)
                val l = (px - item.w / 2).roundToInt()
                val t = (py - item.h / 2).roundToInt()
                canvas.drawBitmap(item.bmp, null, Rect(l, t, l + item.w, t + item.h), paint)
            }
        }
        holder.unlockCanvasAndPost(canvas)
        choreographer.postFrameCallback(this)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        // Helpers to convert screen → world
        fun toWorldX(sx: Float) = (sx - width / 2f) / pxPerMeter
        fun toWorldY(sy: Float) = (height - sy - 100f) / pxPerMeter

        when (event.action) {
            MotionEvent.ACTION_DOWN -> {
                val sx = event.x
                val sy = event.y

                // Hit-test source
                val wx = Box2DEngineNativeBridge.getBodyX(sourceIdx)
                val wy = Box2DEngineNativeBridge.getBodyY(sourceIdx)
                val px = wx * pxPerMeter + width / 2f
                val py = height - (wy * pxPerMeter + 100f)
                val item  = objects.first { it.idx == sourceIdx }
                val halfW = item.w / 2f
                val halfH = item.h / 2f

                if (sx in (px - halfW)..(px + halfW) && sy in (py - halfH)..(py + halfH)) {
                    isDragging   = true
                    dragLastX    = toWorldX(sx)
                    dragLastY    = toWorldY(sy)
                    dragPrevTime = event.eventTime
                    dragVelocityX = 0f
                    dragVelocityY = 0f

                    // zero velocity immediately
                    physicsHandler.post {
                        Box2DEngineNativeBridge.setVelocity(sourceIdx, 0f, 0f)
                    }
                    return true
                }
            }

            MotionEvent.ACTION_MOVE -> if (isDragging) {
                val now = event.eventTime

                // convert & clamp
                var wx = toWorldX(event.x).coerceIn(
                    Box2DEngineNativeBridge.getLeftX(),
                    Box2DEngineNativeBridge.getRightX()
                )
                var wy = toWorldY(event.y).coerceIn(
                    Box2DEngineNativeBridge.getGroundY(),
                    Box2DEngineNativeBridge.getRoofY()
                )

                // prevent static overlap
                for (rect in staticWorldRegions) {
                    if (wx in rect.minX..rect.maxX && wy in rect.minY..rect.maxY) {
                        wx = dragLastX
                        wy = dragLastY
                        break
                    }
                }

                // sample velocity
                val dt = ((now - dragPrevTime) / 1000f).coerceAtLeast(1/60f)
                dragVelocityX = (wx - dragLastX) / dt
                dragVelocityY = (wy - dragLastY) / dt

                // teleport + zero out physics velocity
                physicsHandler.post {
                    Box2DEngineNativeBridge.setVelocity(sourceIdx, 0f, 0f)
                    Box2DEngineNativeBridge.replaceBody(sourceIdx, wx, wy)
                }

                // update trackers
                dragLastX    = wx
                dragLastY    = wy
                dragPrevTime = now
                return true
            }

            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> if (isDragging) {
                isDragging = false

                physicsHandler.post {
                    // final teleport
                    Box2DEngineNativeBridge.replaceBody(sourceIdx, dragLastX, dragLastY)
                    // apply launch velocity
                    Box2DEngineNativeBridge.setVelocity(sourceIdx, dragVelocityX, dragVelocityY)
                }
                return true
            }
        }

        return super.onTouchEvent(event)
    }

    private fun loadAndPrepareBackground(resId: Int) {
        val original = BitmapFactory.decodeResource(resources, resId)
        val viewW = width; val viewH = height
        val scale = maxOf(viewW/original.width.toFloat(),viewH/original.height.toFloat())
        val scaled = original.scale((original.width*scale).toInt(),(original.height*scale).toInt())
        val xOff=(scaled.width-viewW)/2; val yOff=(scaled.height-viewH)/2
        backgroundBmp=Bitmap.createBitmap(scaled,xOff,yOff,viewW,viewH)
        if(scaled!==original) original.recycle()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {}
    override fun surfaceDestroyed(holder: SurfaceHolder) {
        choreographer.removeFrameCallback(this)
        physicsThread.quitSafely(); physicsThread.join()
        Box2DEngineNativeBridge.destroyWorld(); objects.clear()
    }
}
