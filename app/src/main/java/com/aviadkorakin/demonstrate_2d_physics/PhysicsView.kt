// PhysicsView.kt
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
import com.aviadkorakin.demonstrate_2d_physics.level_manager.Level
import com.aviadkorakin.demonstrate_2d_physics.level_manager.ResourceMap
import kotlin.math.pow
import kotlin.math.roundToInt
import androidx.core.graphics.toColorInt

class PhysicsView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs),
    SurfaceHolder.Callback,
    Choreographer.FrameCallback {

    enum class ObjType { PILLAR, SHELF, STATIC_BLOCK, TARGET, OBSTACLE, SOURCE }

    // ── Enqueue before world is ready ─────────────────────────────────────────
    private val pendingAdds = mutableListOf<() -> Unit>()
    private var worldReady = false
    fun enqueue(bodyCreator: () -> Unit) {
        if (worldReady) bodyCreator() else pendingAdds += bodyCreator
    }

    // ── Shared state ──────────────────────────────────────────────────────────
    private var score = 0
    private var dragCount = 0
    private var totalTargetScore = 0
    private var hasWon = false
    private var originalSourceX = 0f
    private var originalSourceY = 0f
    private val bmpMap   = mutableMapOf<ObjType, List<Bitmap>>()
    private val paintMap = mutableMapOf<ObjType, Paint>()
    private val defaultPaint = Paint(Paint.ANTI_ALIAS_FLAG)
    private val scorePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color    = Color.WHITE
        textSize = 48f
        typeface = Typeface.DEFAULT_BOLD
    }

    internal var pxPerMeter = 50f

    private data class RenderItem(
        val idx: Int,
        val type: ObjType,
        val w: Int,
        val h: Int,
        val oBmp: Bitmap?,
        val oPaint: Paint?
    )
    private val objects = mutableListOf<RenderItem>()
    private var sourceIdx = -1
    private var sourceRadius = 0f
    private val staticRects = mutableMapOf<Int, WorldRect>()

    // record each static body's actual world rectangle
    private data class WorldRect(val minX: Float, val minY: Float, val maxX: Float, val maxY: Float)
    private val staticRegions = mutableListOf<WorldRect>()
    private val obstacleRects = mutableMapOf<Int, WorldRect>()

    // ── Background support ────────────────────────────────────────────────────
    private var backgroundBmp: Bitmap? = null
    private fun loadBackground(resId: Int) {
        val orig = BitmapFactory.decodeResource(resources, resId)
        val scale = maxOf(width / orig.width.toFloat(), height / orig.height.toFloat())
        val scaled = orig.scale((orig.width * scale).toInt(), (orig.height * scale).toInt())
        val xOff = (scaled.width  - width ) / 2
        val yOff = (scaled.height - height) / 2
        backgroundBmp = Bitmap.createBitmap(scaled, xOff, yOff, width, height)
        if (scaled != orig) orig.recycle()
    }

    // ── World‐bounds cache ─────────────────────────────────────────────────────
    private var worldLeftX   = 0f
    private var worldRightX  = 0f
    private var worldGroundY = 0f
    private var worldRoofY   = 0f

    // ── Physics / threading ────────────────────────────────────────────────────
    private val choreo = Choreographer.getInstance()
    private lateinit var physicsThread: HandlerThread
    private lateinit var physicsHandler: Handler
    private var lastNanos = 0L

    // ── Drag state ─────────────────────────────────────────────────────────────
    private var isDragging    = false
    private var dragCooldown  = false
    private var dragStartTime = 0L
    private var dragX = 0f
    private var dragY = 0f
    private var prevTime = 0L
    private var velX = 0f
    private var velY = 0f

    // ── Win callback ──────────────────────────────────────────────────────────
    private var winListener: (() -> Unit)? = null
    fun setOnWinListener(cb: () -> Unit) { winListener = cb }

    // ── UI‐thread handler for cooldown ────────────────────────────────────────
    private val uiHandler = Handler(context.mainLooper)

    init { holder.addCallback(this) }

    /** Supply bitmaps for dynamic types. */
    fun setBitmaps(type: ObjType, bitmaps: List<Bitmap>) {
        bmpMap[type] = bitmaps
    }

    /** Supply Paints for static shapes. */
    fun setPaint(type: ObjType, paint: Paint) {
        paintMap[type] = paint
    }

    // ── SurfaceHolder.Callback ────────────────────────────────────────────────

    override fun surfaceCreated(holder: SurfaceHolder) {
        // reset view‐side state
        objects.clear()
        staticRegions.clear()
        hasWon = false
        score = 0
        totalTargetScore = 0

        // 2) preload your background
        loadBackground(R.drawable.background)

        // 3) start the physics thread & handler *before* running enqueued work
        physicsThread  = HandlerThread("PhysicsThread").apply { start() }
        physicsHandler = Handler(physicsThread.looper)

        // 4) now drain the queue—initLevel(level) will safely post to physicsHandler
        worldReady = true
        pendingAdds.forEach { it() }
        pendingAdds.clear()


        // 6) and finally kick off your render loop
        choreo.postFrameCallback(this)
    }

    fun initLevel(level: Level) {

        hasWon = false
        staticRects.clear()
        obstacleRects.clear()
        totalTargetScore = 0
        score            = 0
        dragCount        = 0
        lastNanos        = 0L
        isDragging       = false        // no drag in progress
        dragCooldown     = false        // allow a fresh drag immediately
        originalSourceX  = 0f           // forget where the last drag started
        originalSourceY  = 0f
        dragX            = 0f           // forget last “current” drag pos
        dragY            = 0f
        prevTime         = 0L           // clear the timestamp used for velocity calc
        velX             = 0f           // zero out any leftover launch velocity
        velY             = 0f

        physicsHandler.removeCallbacksAndMessages(null)
        Box2DEngineNativeBridge.destroyWorld()
        Box2DEngineNativeBridge.initWorld(level.world.gravity.gx, level.world.gravity.gy)

        // 2) Clear all view-side state
        synchronized(objects) { objects.clear() }

        // ── 0) decode & install per-type sprite lists (will be empty if JSON omitted)
        val targetBmps = level.bitmaps.target.mapNotNull { name ->
            ResourceMap.drawables[name]?.let { resId ->
                BitmapFactory.decodeResource(context.resources, resId)
            }
        }
        setBitmaps(ObjType.TARGET, targetBmps)

        val obstacleBmps = level.bitmaps.obstacle.mapNotNull { name ->
            ResourceMap.drawables[name]?.let { resId ->
                BitmapFactory.decodeResource(context.resources, resId)
            }
        }
        setBitmaps(ObjType.OBSTACLE, obstacleBmps)

        val staticBmps = level.bitmaps.staticBlock.mapNotNull { name ->
            ResourceMap.drawables[name]?.let { resId ->
                BitmapFactory.decodeResource(context.resources, resId)
            }
        }
        setBitmaps(ObjType.STATIC_BLOCK, staticBmps)

        val sourceBmps = level.bitmaps.source.mapNotNull { name ->
            ResourceMap.drawables[name]?.let { resId ->
                BitmapFactory.decodeResource(context.resources, resId)
            }
        }
        setBitmaps(ObjType.SOURCE, sourceBmps)

        // ── 1) set gravity on physics thread
        physicsHandler.post {
            Box2DEngineNativeBridge.setGravity(
                level.world.gravity.gx,
                level.world.gravity.gy
            )
        }

        // ── 2) compute pxPerMeter from all object spans
        val allXs = listOf(
            level.objects.pillars.flatMap    { listOf(it.x - it.halfW, it.x + it.halfW) },
            level.objects.staticBlocks.flatMap{ listOf(it.x - it.halfW, it.x + it.halfW) },
            level.objects.shelves.flatMap    { listOf(it.x - it.halfW, it.x + it.halfW) },
            level.objects.obstacles.flatMap  { listOf(it.x - it.halfW, it.x + it.halfW) },
            level.objects.targets.flatMap    { listOf(it.x - it.radius, it.x + it.radius) },
            listOf(
                level.objects.source.x - level.objects.source.radius,
                level.objects.source.x + level.objects.source.radius
            )
        ).flatten()
        val minX = allXs.minOrNull() ?: -7f
        val maxX = allXs.maxOrNull() ?:  7f
        pxPerMeter = width.toFloat() / (maxX - minX)

        // ── 3) add ground, roof, walls
        val physW = width.toFloat()  / pxPerMeter
        val physH = height.toFloat() / pxPerMeter
        val halfScreenWorldW = (width / 2f) / pxPerMeter
        val screenLeftX  = -halfScreenWorldW
        val screenRightX =  halfScreenWorldW

        Box2DEngineNativeBridge.addGround(0f,       physW, 0f,    0.5f)
        Box2DEngineNativeBridge.addRoof  (physH,    physW, 0f,    0.5f)
        Box2DEngineNativeBridge.addLeftWall (screenLeftX,  physH, 0f, 0.5f)
        Box2DEngineNativeBridge.addRightWall(screenRightX, physH, 0f, 0.5f)

        worldLeftX   = screenLeftX
        worldRightX  = screenRightX
        worldGroundY = Box2DEngineNativeBridge.getGroundY()
        worldRoofY   = Box2DEngineNativeBridge.getRoofY()
        Box2DEngineNativeBridge.resetScore()

        // ── 4) JSON-defined pillars & shelves (with optional JSON color)
        level.objects.pillars.forEach { def ->
            val paint = def.color?.toColorInt()
                ?.let { c ->
                    Paint(Paint.ANTI_ALIAS_FLAG).apply {
                        style = Paint.Style.FILL
                        color = c
                    }
                }
                ?: paintMap[ObjType.PILLAR]
            addPillar(def.x, def.y, def.halfW, def.halfH, paint)
        }

        level.objects.shelves.forEach { def ->
            val paint = def.color?.toColorInt()
                ?.let { c ->
                    Paint(Paint.ANTI_ALIAS_FLAG).apply {
                        style = Paint.Style.FILL
                        color = c
                    }
                }
                ?: paintMap[ObjType.SHELF]
            addShelf(def.x, def.y, def.halfW, def.halfH, paint)
        }

        // ── 5) JSON-defined staticBlocks (bitmap + paint)
        level.objects.staticBlocks.forEach { def ->
            // optional bitmap
            val bmp = def.spriteIndex
                ?.takeIf { it in staticBmps.indices }
                ?.let { staticBmps[it] }

            // paint (JSON color or fallback)
            val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
                style = Paint.Style.FILL
                color = def.color?.let(Color::parseColor)
                    ?: (paintMap[ObjType.STATIC_BLOCK]?.color ?: Color.DKGRAY)
            }
            addStaticBlock(def.x, def.y, def.halfW, def.halfH, bmp, paint)
        }

        // ── 6) JSON-defined dynamic targets, obstacles, source
        level.objects.targets.forEach { def ->
            val bmp = targetBmps.getOrNull(def.spriteIndex)
            addTarget(def.x, def.y, def.radius, def.score, bmp!!)
        }

        level.objects.obstacles.forEach { def ->
            val bmp = obstacleBmps.getOrNull(def.spriteIndex)
            addObstacle(def.x, def.y, def.halfW, def.halfH, bmp!!)
        }

        level.objects.source.let { src ->
            addSource(src.x, src.y, src.radius)
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {}
    override fun surfaceDestroyed(holder: SurfaceHolder) {
        choreo.removeFrameCallback(this)
        physicsThread.quitSafely()
        physicsThread.join()
        Box2DEngineNativeBridge.destroyWorld()
        objects.clear()
        staticRegions.clear()
    }

    // ── Body‐creation helpers ───────────────────────────────────────────────────
    private fun createItem(
        type: ObjType,
        idx: Int,
        worldCX: Float,
        worldCY: Float,
        halfW: Float,
        halfH: Float,
        oBmp: Bitmap?,
        oPaint: Paint?
    ) {
        if (type == ObjType.PILLAR || type == ObjType.SHELF || type == ObjType.STATIC_BLOCK) {
            val rect = WorldRect(
                worldCX - halfW,
                worldCY - halfH,
                worldCX + halfW,
                worldCY + halfH
            )
            staticRects[idx] = rect
        }
        val wPx = (halfW*2f*pxPerMeter).roundToInt()
        val hPx = (halfH*2f*pxPerMeter).roundToInt()
        objects += RenderItem(idx, type, wPx, hPx, oBmp, oPaint)
    }

    fun addPillar(
        x: Float,
        y: Float,
        halfW: Float,
        halfH: Float,
        paint: Paint? = null
    ) = enqueue {
        val density = computeDensity(halfW, halfH)
        val id = Box2DEngineNativeBridge.createStaticObstacle(
            1, x, y, halfW, halfH, density, 0.5f, 0f
        )
        // now pass the paint through to createItem:
        createItem(ObjType.PILLAR, id, x, y, halfW, halfH, /* oBmp = */ null, paint)
    }

    /**
     * Add a static shelf with an optional specific Paint chosen by the caller.
     */
    fun addShelf(
        x: Float,
        y: Float,
        halfW: Float,
        halfH: Float,
        paint: Paint? = null
    ) = enqueue {
        val density = computeDensity(halfW, halfH)
        val id = Box2DEngineNativeBridge.createStaticObstacle(
            1, x, y, halfW, halfH, density, 0.5f, 0f
        )
        createItem(ObjType.SHELF, id, x, y, halfW, halfH, /* oBmp = */ null, paint)
    }
    /**
     * Add a static block, optionally skinned with a bitmap *and* painted with a custom Paint.
     *
     * @param x       world X-coordinate of center
     * @param y       world Y-coordinate of center
     * @param halfW   half-width in world units
     * @param halfH   half-height in world units
     * @param bmp     optional Bitmap to draw
     * @param paint   optional Paint to draw (fill or stroke)
     */
    fun addStaticBlock(
        x: Float,
        y: Float,
        halfW: Float,
        halfH: Float,
        bmp: Bitmap? = null,
        paint: Paint? = null
    ) = enqueue {
        // 1) create the Box2D body
        val density = computeDensity(halfW, halfH)
        val id = Box2DEngineNativeBridge.createStaticObstacle(
            /* categoryBits=*/1,
            x, y,
            halfW, halfH,
            density,
            /* restitution=*/0.5f,
            /* friction=*/0f
        )

        // 2) capture its world‐rect and add to our render list
        createItem(
            type    = ObjType.STATIC_BLOCK,
            idx     = id,
            worldCX = x,
            worldCY = y,
            halfW   = halfW,
            halfH   = halfH,
            oBmp    = bmp,
            oPaint  = paint ?: paintMap[ObjType.STATIC_BLOCK]
        )
    }
    /**
     * Add a dynamic target with a specific bitmap chosen by the caller.
     */
    fun addTarget(
        x: Float,
        y: Float,
        radius: Float,
        score: Int,
        bmp: Bitmap
    ) = enqueue {
        val density = computeDensity(radius, radius)
        val id = Box2DEngineNativeBridge.createDynamicTarget(
            0,           // shapeType / categoryBits
            x,           // world X
            y,           // world Y
            radius,      // a (half-width)
            radius,      // b (half-height)
            density,     // density
            0.3f,        // friction
            0.8f,        // restitution
            score        // score for this specific target
        )
        if( id != -1 )
        {
            createItem(
                type    = ObjType.TARGET,
                idx     = id,
                worldCX = x,
                worldCY = y,
                halfW   = radius,
                halfH   = radius,
                oBmp    = bmp,
                oPaint  = null
            )
            totalTargetScore += score
        }
    }

    /**
     * Add a dynamic obstacle with a specific bitmap chosen by the caller.
     */
    fun addObstacle(
        x: Float,
        y: Float,
        halfW: Float,
        halfH: Float,
        bmp: Bitmap
    ) = enqueue {
        val density = computeDensity(halfW, halfH)
        val id = Box2DEngineNativeBridge.createDynamicObstacle(
            1,        // shapeType / categoryBits
            x,        // world X
            y,        // world Y
            halfW,    // a (half-width)
            halfH,    // b (half-height)
            density,  // density
            0.3f,     // friction
            0.8f      // restitution
        )
        if( id != -1 ) {
            createItem(
                type = ObjType.OBSTACLE,
                idx = id,
                worldCX = x,
                worldCY = y,
                halfW = halfW,
                halfH = halfH,
                oBmp = bmp,
                oPaint = null
            )
        }
    }
    fun addSource(x: Float, y: Float, radius: Float) = enqueue {
        val density = computeDensity(radius, radius)
        val id = Box2DEngineNativeBridge.createDynamicSource(
            0,           // shapeType / categoryBits
            x,           // world X
            y,           // world Y
            radius,      // a (half-width)
            radius,      // b (half-height)
            density,     // density
            0.2f,        // friction
            0.5f         // restitution
        )
        sourceIdx    = id
        sourceRadius = radius
        val bmp = bmpMap[ObjType.SOURCE]?.random()
        createItem(
            type    = ObjType.SOURCE,
            idx     = id,
            worldCX = x,
            worldCY = y,
            halfW   = radius,
            halfH   = radius,
            oBmp    = bmp,
            oPaint  = null
        )

        Box2DEngineNativeBridge.setVelocity(id, 0f, 0f)
    }

    // ── FrameCallback ─────────────────────────────────────────────────────────
    override fun doFrame(frameTimeNanos: Long) {
        if (lastNanos == 0L) lastNanos = frameTimeNanos
        val dt = (frameTimeNanos - lastNanos)/1e9f
        lastNanos = frameTimeNanos

        physicsHandler.post {
            val removed = Box2DEngineNativeBridge.stepAndGetRemoved(dt)
            synchronized(objects) { objects.removeAll { it.idx in removed } }
            score = Box2DEngineNativeBridge.getScore()
            if (!hasWon && totalTargetScore >0 && score>=totalTargetScore) {
                hasWon = true
                winListener?.invoke()
            }
        }

        val canvas = holder.lockCanvas() ?: return
        backgroundBmp?.let { canvas.drawBitmap(it, 0f, 0f, defaultPaint) }
            ?: canvas.drawColor(Color.WHITE)

        // draw bodies
        val flat = Box2DEngineNativeBridge.getAllBodyPositions()
        val pos  = mutableMapOf<Int, Pair<Float,Float>>()
        var i = 0
        while(i+2<flat.size) {
            pos[flat[i].toInt()] = flat[i+1] to flat[i+2]
            i += 3
        }

        // ── rebuild only the obstacle rectangles ──────────────────────────
              obstacleRects.clear()
               synchronized(objects) {
                    for ((idx, type, wPx, hPx, _, _) in objects) {
                           if (type == ObjType.OBSTACLE) {
                                val halfW = (wPx  / pxPerMeter) / 2f
                                 val halfH = (hPx  / pxPerMeter) / 2f
                                 val (wx, wy) = pos[idx] ?: continue
                                obstacleRects[idx] = WorldRect(
                                      wx - halfW, wy - halfH,
                                       wx + halfW, wy + halfH
                                            )
                               }
                        }
                  }


        synchronized(objects) {
            for ((idx,type,w,h,oBmp,oPaint) in objects) {
                pos[idx]?.let { (wx,wy) ->
                    val px   = wx*pxPerMeter + width/2f
                    val py   = height - (wy*pxPerMeter +100f)
                    val left = (px - w/2f).roundToInt()
                    val top  = (py - h/2f).roundToInt()
                    val dst  = Rect(left, top, left+w, top+h)

                    when {

                        type == ObjType.OBSTACLE && w != h -> {
                            canvas.drawRect(left.toFloat(), top.toFloat(),
                                (left+w).toFloat(), (top+h).toFloat(),
                                paintMap[ObjType.OBSTACLE] ?: defaultPaint)
                        }

                        oPaint != null -> canvas.drawRect(
                            left.toFloat(), top.toFloat(),
                            (left+w).toFloat(), (top+h).toFloat(),
                            oPaint
                        )
                        oBmp != null   -> canvas.drawBitmap(oBmp, null, dst, defaultPaint)
                        else -> oBmp?.let { canvas.drawBitmap(it, null, dst, defaultPaint)
                        }
                    }
                }
            }
        }

        canvas.drawText("Score: $score", 20f, 60f, scorePaint)
        holder.unlockCanvasAndPost(canvas)
        choreo.postFrameCallback(this)
    }

    // ── Drag & launch with hit‐test, immediate collision + cooldown ──────────────


    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (dragCooldown && event.action == MotionEvent.ACTION_DOWN) return false

        return when (event.action) {
            MotionEvent.ACTION_DOWN   -> handleActionDown(event)
            MotionEvent.ACTION_MOVE   -> handleActionMove(event)
            MotionEvent.ACTION_UP,
            MotionEvent.ACTION_CANCEL -> handleActionUp(event)
            else                      -> super.onTouchEvent(event)
        }
    }

    private fun handleActionDown(event: MotionEvent): Boolean {
        val (wx, wy) = screenToWorld(event.x, event.y)
        val (srcX, srcY) = Box2DEngineNativeBridge.getBodyPosition(sourceIdx)

        // must tap inside the source circle
        if ((wx - srcX).pow(2) + (wy - srcY).pow(2) > sourceRadius*sourceRadius) {
            return false
        }

        dragCount++
        originalSourceX = srcX
        originalSourceY = srcY

        isDragging    = true
        dragStartTime = event.eventTime
        dragX = wx; dragY = wy;  prevTime = dragStartTime

        physicsHandler.post {
            Box2DEngineNativeBridge.replaceBody(sourceIdx, wx, wy)
            Box2DEngineNativeBridge.setVelocity(sourceIdx, 0f, 0f)
            Box2DEngineNativeBridge.setGravityScale(sourceIdx, 0f)
            Box2DEngineNativeBridge.extrasClearContacts()
        }
        return true
    }

    private fun handleActionMove(event: MotionEvent): Boolean {
        if (!isDragging) return false

        val now = event.eventTime
        if (now - dragStartTime > 500L) {
            endDragWithCooldown()
            return true
        }

        val (newX, newY) = screenToWorld(event.x, event.y)

        // 1) Invalid if sweeping through a static rect...
        val hitStatic = staticRects.values.any { rect ->
            segmentIntersectsRect(dragX, dragY, newX, newY, rect)
        }
        val hitObstacle = obstacleRects.values.any  { r -> segmentIntersectsRect(dragX, dragY, newX, newY, r) }


        // 2) Invalid if new position would lie outside the allowed bounds
        val outOfBounds = newX < worldLeftX+sourceRadius
                || newX > worldRightX-sourceRadius
                || newY < worldGroundY+sourceRadius/2
                || newY > worldRoofY-sourceRadius

        if (outOfBounds || hitStatic  || hitObstacle ) {
            // snap back to original and cancel
            physicsHandler.post {
                Box2DEngineNativeBridge.replaceBody(
                    sourceIdx,
                    originalSourceX,
                    originalSourceY
                )
                Box2DEngineNativeBridge.setVelocity(sourceIdx, 0f, 0f)
                Box2DEngineNativeBridge.setGravityScale(sourceIdx, 1f)
            }
            endDragWithCooldown()
            return true
        }

        // otherwise, commit the move
        physicsHandler.post {
            Box2DEngineNativeBridge.replaceBody(sourceIdx, newX, newY)
        }
        val dt = (now - prevTime).coerceAtLeast(1L) / 1000f
        velX = (newX - dragX) / dt
        velY = (newY - dragY) / dt
        dragX = newX; dragY = newY; prevTime = now
        return true
    }

    private fun handleActionUp(event: MotionEvent): Boolean {
        if (!isDragging) return false
        endDragWithCooldown()
        return true
    }

// Helpers

    private fun screenToWorld(sx: Float, sy: Float): Pair<Float,Float> {
        val wx = (sx - width/2f) / pxPerMeter
        val wy = (height - sy - 100f) / pxPerMeter
        return wx to wy
    }

    private fun Float.pow(exp: Int) = this.toDouble().pow(exp).toFloat()


    /** Drop & launch + 2s cooldown before next pick‐up */
    private fun endDragWithCooldown() {
        isDragging = false
        physicsHandler.post {
            Box2DEngineNativeBridge.replaceBody(sourceIdx, dragX, dragY)
            Box2DEngineNativeBridge.setGravityScale(sourceIdx, 1f)
            Box2DEngineNativeBridge.setVelocity(sourceIdx, velX, velY)
        }
        dragCooldown = true
        uiHandler.postDelayed({ dragCooldown = false }, 500L)
    }

    /** Destroy and visually remove a static body. */
    fun removeStatic(idx: Int?) {
        idx ?: return                     // if idx is null, bail out

        physicsHandler.post {
            Box2DEngineNativeBridge.destroyBody(idx)  // idx is now non-null
        }

        synchronized(objects) {
            // drop from draw list
            objects.removeAll { it.idx == idx }
        }

        // drop from your UI-side hit test
        staticRects.remove(idx)
    }

    // ── Reachability check ────────────────────────────────────────────────────


    fun findAnyBlockingStatic(): Int? {
        // build current positions…
        val pos = Box2DEngineNativeBridge.getBodyPosition(sourceIdx)
        val sx  = pos[0]; val sy = pos[1]

        val flat = Box2DEngineNativeBridge.getAllBodyPositions()
        val dyn  = mutableMapOf<Int, Pair<Float,Float>>().apply {
            var i = 0
            while (i + 2 < flat.size) {
                put(flat[i].toInt(), flat[i+1] to flat[i+2])
                i += 3
            }
        }

        // test each static rect
        for (idx in staticRects.keys.toList()) {
            // remove it
            val saved = staticRects.remove(idx) ?: continue
            // see if now all targets are reachable
            val ok = validateAllTargetsReachable()
            // put it back
            staticRects[idx] = saved
            if (ok) return idx
        }
        // if none helped, just return the first one (or null if empty)
        return staticRects.keys.firstOrNull()
    }

    fun validateAllTargetsReachable(): Boolean {
        // 1) get source position
        val pos = Box2DEngineNativeBridge.getBodyPosition(sourceIdx)
        val sx = pos[0]; val sy = pos[1]

        // 2) build map of dynamic bodies
        val flat = Box2DEngineNativeBridge.getAllBodyPositions()
        val dynPos = mutableMapOf<Int, Pair<Float,Float>>()
        var i = 0
        while (i + 2 < flat.size) {
            dynPos[flat[i].toInt()] = flat[i+1] to flat[i+2]
            i += 3
        }

        // 3) for each target, cast a line against *all* staticRects
        return objects
            .filter { it.type == ObjType.TARGET }
            .all { item ->
                dynPos[item.idx]?.let { (tx, ty) ->
                    staticRects.values.none { segmentIntersectsRect(sx, sy, tx, ty, it) }
                } ?: true
            }
    }
    private fun segmentIntersectsRect(
        x1:Float,y1:Float,x2:Float,y2:Float, rect: WorldRect
    ): Boolean {
        if (x1<rect.minX && x2<rect.minX) return false
        if (x1>rect.maxX && x2>rect.maxX) return false
        if (y1<rect.minY && y2<rect.minY) return false
        if (y1>rect.maxY && y2>rect.maxY) return false
        var t0=0f; var t1=1f; val dx=x2-x1; val dy=y2-y1
        fun clip(p:Float,q:Float):Boolean {
            if (p==0f) return q>=0f
            val r=q/p
            when {
                p<0 && r>t1 -> return false
                p<0 && r>t0 -> t0=r
                p>0 && r<t0 -> return false
                p>0 && r<t1 -> t1=r
            }
            return true
        }
        return clip(-dx, x1-rect.minX)
                && clip( dx, rect.maxX-x1)
                && clip(-dy, y1-rect.minY)
                && clip( dy, rect.maxY-y1)
    }

    private fun computeDensity(halfW: Float, halfH: Float): Float {
        val area = (halfW * 2f) * (halfH * 2f)
        val baseDensity = 0.5f          // tweak this “unit” density if you like
        return baseDensity * area
    }
    fun setWorldGravity(gx: Float, gy: Float) {
        physicsHandler.post {
            Box2DEngineNativeBridge.setGravity(gx, gy)
        }
    }
    fun getDragCount(): Int = dragCount
    fun getScore(): Int = score

}