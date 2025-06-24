// Level.kt
package com.aviadkorakin.demonstrate_2d_physics.level_manager

/**
 * Holds the JSON-driven lists of all sprite resource names
 * that this level will use.
 */
data class LevelBitmaps(
    val target:      List<String> = emptyList(),
    val obstacle:    List<String> = emptyList(),
    val staticBlock: List<String> = emptyList(),
    val source:      List<String> = emptyList()
)

/**
 * Full level definition, including paints for pillars & shelves.
 */
data class Level(
    val levelName: String,
    val world:     WorldConfig,
    val bitmaps:   LevelBitmaps = LevelBitmaps(),
    val objects:   ObjectsConfig
)

data class WorldConfig(
    val gravity: Vec2
)
data class Vec2(
    val gx: Float,
    val gy: Float
)

data class ObjectsConfig(
    val pillars:       List<BoxDef>      = emptyList(),
    val shelves:       List<BoxDef>      = emptyList(),
    val staticBlocks:  List<BoxDef>      = emptyList(),
    val targets:       List<TargetDef>   = emptyList(),
    val obstacles:     List<ObstacleDef> = emptyList(),
    val source:        SourceDef
)

/**
 * A static pillar/shelf/block.
 *
 *  - color: optional hex string (e.g. "#333333"). If null, uses default Paint.
 *  - spriteIndex: still available for staticBlocks if you want bitmaps there.
 */
data class BoxDef(
    val x:           Float,
    val y:           Float,
    val halfW:       Float,
    val halfH:       Float,
    val color:       String? = null,
    val spriteIndex: Int?   = null
)

data class TargetDef(
    val x:           Float,
    val y:           Float,
    val radius:      Float,
    val score:       Int,
    val spriteIndex: Int
)

data class ObstacleDef(
    val x:           Float,
    val y:           Float,
    val halfW:       Float,
    val halfH:       Float,
    val spriteIndex: Int
)

data class SourceDef(
    val x:      Float,
    val y:      Float,
    val radius: Float
)