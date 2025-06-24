// MainActivity.kt
package com.aviadkorakin.demonstrate_2d_physics

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<Button>(R.id.startButton).setOnClickListener {
            startActivity(Intent(this, GameActivity::class.java).apply {
                putExtra("LEVEL_NUM", 1)
            })
        }
    }
}