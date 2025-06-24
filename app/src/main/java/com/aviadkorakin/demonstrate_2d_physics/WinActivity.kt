// WinActivity.kt
package com.aviadkorakin.demonstrate_2d_physics

import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class WinActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_win)

        // 1) Read passed‐in extras (defaults to 0 if missing)
        val drags = intent.getIntExtra("DRAG_COUNT", 0)
        val score = intent.getIntExtra("FINAL_SCORE", 0)

        // 2) Find and populate your TextViews
        findViewById<TextView>(R.id.dragCountText).text = "Throws: $drags"
        findViewById<TextView>(R.id.scoreText).text     = "Score: $score"
    }
}