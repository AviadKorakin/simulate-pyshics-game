// GameActivity.kt
package com.aviadkorakin.demonstrate_2d_physics

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.aviadkorakin.demonstrate_2d_physics.level_manager.Level
import com.google.gson.Gson
import java.io.IOException

class GameActivity : AppCompatActivity() {
    private lateinit var physicsView: PhysicsView
    private val gson = Gson()

    private var currentLevel = 1

    // accumulate per-level stats
    private val levelDrags  = mutableListOf<Int>()
    private val levelScores = mutableListOf<Int>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)
        physicsView = findViewById(R.id.physicsView)

        // load the first level; if missing, show summary
        loadLevel(currentLevel)

        physicsView.setOnWinListener {
            // record stats for completed level
            levelDrags.add(physicsView.getDragCount())
            levelScores.add(physicsView.getScore())

            // advance to next level; loadLevel will show summary if file is missing
            currentLevel++
            loadLevel(currentLevel)
        }
    }

    /**
     * Attempts to load level n from assets. If the JSON file isn't found, shows summary and exits.
     */
    private fun loadLevel(n: Int) {
        val filename = "levels/level$n.json"
        try {
            val json = assets.open(filename).bufferedReader().use { it.readText() }
            val level = gson.fromJson(json, Level::class.java)

            physicsView.enqueue { physicsView.initLevel(level) }
            physicsView.post {
                if (!physicsView.validateAllTargetsReachable()) {
                    physicsView.findAnyBlockingStatic()?.let { physicsView.removeStatic(it) }
                }
            }
        } catch (e: IOException) {
            // no more levels: finish with summary
            showSummary()
        }
    }

    private fun showSummary() {
        val totalDrags = levelDrags.sum()
        val totalScore = levelScores.sum()

        val intent = Intent(this, WinActivity::class.java).apply {
            putExtra("DRAG_COUNT", totalDrags)
            putExtra("FINAL_SCORE", totalScore)
        }
        startActivity(intent)
        finish()
    }
}
