package com.aviadkorakin.demonstrate_2d_physics.level_manager

import com.aviadkorakin.demonstrate_2d_physics.R

object ResourceMap {
    /** All the drawable keys used by every level. */
    val drawables: Map<String, Int> = mapOf(
        "ic_target"    to R.drawable.ic_target,
        "ic_target2"   to R.drawable.ic_target2,
        "ic_target3"   to R.drawable.ic_target3,
        "ic_target4"   to R.drawable.ic_target4,

        "ic_obstacle"  to R.drawable.ic_obstacle,
        "ic_obstacle2" to R.drawable.ic_obstacle2,
        "ic_obstacle3" to R.drawable.ic_obstacle3,

        /** New: your source (bomb) icon(s) */
        "ic_source"    to R.drawable.ic_source,
    )

    /** Per‐target sprite → score mapping. */
    val targetScores: Map<String, Int> = mapOf(
        "ic_target"    to 1,
        "ic_target2"   to 2,
        "ic_target3"   to 3,
        "ic_target4"   to 5
    )

    /** (Optional) if you ever want per‐source scores, add here too. */
    val sourceScores: Map<String, Int> = mapOf(
        "ic_source"    to 0,    // typically 0 since it's not collectible
    )

    /** Helpers */
    fun resIdFor(key: String): Int?      = drawables[key]
    fun scoreForTarget(key: String): Int = targetScores[key] ?: 0
    fun scoreForSource(key: String): Int = sourceScores[key] ?: 0
}