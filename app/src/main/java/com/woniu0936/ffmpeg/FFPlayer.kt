package com.woniu0936.ffmpeg

import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class FFPlayer : SurfaceHolder.Callback {

    private var dataSource: String? = null
    private lateinit var surfaceView: SurfaceView
    private var onPrepareListener: OnPrepareListener? = null

    fun setDataSource(dataSource: String) {
        this.dataSource = dataSource;
    }

    /**
     * 准备要播放的视频
     */
    fun prepare() {
//        Log.d("FFMPEG_JAVA", "FFPlayer prepare")
        dataSource?.let {
            nativePrepare(it)
        }
    }

    fun setOnPrepareListener(listener: OnPrepareListener) {
        onPrepareListener = listener
    }

    /**
     * 开始播放视频
     */
    fun start() {
        nativeStart()
    }

    /**
     * 停止播放视频
     */
    fun stop() {

    }

    /**
     * 释放资源
     */
    fun release() {

    }

    fun onPrepare() {
        Log.d("FFMPEG_JAVA", "FFPlayer onPrepare")
        onPrepareListener?.onPrepare()
    }

    fun onError(code: Int) {
        Log.d("FFMPEG_JAVA", "FFPlayer onError: $code")
    }

    /**
     * 设置播放视频的画布
     */
    fun setSurfaceView(surfaceView: SurfaceView) {
        this.surfaceView = surfaceView
        surfaceView.holder.addCallback(this)
    }

    /**
     * 画布创建好了
     */
    override fun surfaceCreated(holder: SurfaceHolder) {
        nativeSetSurface(holder.surface)
    }

    /**
     * 画布发生了变化
     */
    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        nativeSetSurface(holder.surface)
    }

    /**
     * 画布销毁
     */
    override fun surfaceDestroyed(holder: SurfaceHolder) {

    }

    /**
     * A native method that is implemented by the 'ffmpeg' native library,
     * which is packaged with this application.
     */
    private external fun stringFromJNI(): String
    private external fun ffmpegInfo(): String

    private external fun nativePrepare(dataSource: String)

    private external fun nativeStart()

    private external fun nativeSetSurface(surface: Surface)

    companion object {
        // Used to load the 'ffmpeg' library on application startup.
        init {
            System.loadLibrary("ffmpeg")
        }
    }

}

interface OnPrepareListener {
    fun onPrepare()
}

