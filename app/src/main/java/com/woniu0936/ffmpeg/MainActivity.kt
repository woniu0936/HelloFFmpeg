package com.woniu0936.ffmpeg

import android.Manifest
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import com.permissionx.guolindev.PermissionX
import com.woniu0936.ffmpeg.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private val mVideoPath =
        Environment.getExternalStorageDirectory().absolutePath + "/Movies/result.mp4"
//        Environment.getExternalStorageDirectory().absolutePath + "/byteflow/one_piece.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        PermissionX.init(this)
            .permissions(Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE)
            .request { allGranted, grantedList, deniedList ->
                if (allGranted) {
                    initPlayer()
                } else {
                    Toast.makeText(this, "缺少磁盘权限", Toast.LENGTH_LONG).show()
                }
            }

        // 播放
        binding.btnStart.setOnClickListener {

        }

        //暂停
        binding.btnStop.setOnClickListener {

        }
    }

    var player: FFPlayer? = null

    private fun initPlayer() {
        player = FFPlayer().apply {
            setDataSource(mVideoPath)
            setSurfaceView(binding.surfaceView)
            setOnPrepareListener(object : OnPrepareListener {
                override fun onPrepare() {
                    Log.d("ffmpeg-java", "player prepare success")
                }
            })
            prepare()
        }
    }

}