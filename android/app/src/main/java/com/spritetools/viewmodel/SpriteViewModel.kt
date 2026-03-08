package com.spritetools.viewmodel

import android.app.Application
import android.graphics.Bitmap
import android.net.Uri
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.spritetools.core.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock

data class SpriteUiState(
    val isLoaded: Boolean = false,
    val fileName: String = "",
    val statusMessage: String = "Open .spr file",
    val statusTimestamp: Long = 0,

    val spriteInfo: SpriteInfo? = null,
    val totalFrames: Int = 0,

    val currentFrame: Int = 0,
    val currentFrameInfo: FrameInfo? = null,
    val currentBitmap: Bitmap? = null,

    val isPlaying: Boolean = false,
    val playbackSpeed: Float = 1.0f,

    val zoom: Float = 1.0f,
    val offsetX: Float = 0f,
    val offsetY: Float = 0f,
    val showChecker: Boolean = true,
    val showProperties: Boolean = false,
    val showToolbar: Boolean = true,

    val groups: List<GroupDisplayInfo> = emptyList(),
    val palette: List<Int>? = null,

    val showAbout: Boolean = false,
    val showExportDialog: Boolean = false
)

data class GroupDisplayInfo(
    val index: Int,
    val info: GroupInfo,
    val frames: List<FrameDisplayInfo>
)

data class FrameDisplayInfo(
    val globalIndex: Int,
    val localIndex: Int,
    val info: FrameInfo
)

class SpriteViewModel(application: Application) : AndroidViewModel(application) {

    companion object {
        private const val TAG = "SpriteViewModel"
    }

    private val manager = SpriteManager()
    private val mutex = Mutex()

    private val _state = MutableStateFlow(SpriteUiState())
    val state: StateFlow<SpriteUiState> = _state.asStateFlow()

    private var playJob: Job? = null

    private val bitmapCache = mutableMapOf<Int, Bitmap>()

    override fun onCleared() {
        super.onCleared()
        playJob?.cancel()
        playJob = null
        bitmapCache.clear()
        manager.close()
    }

    fun loadFile(uri: Uri) {
        viewModelScope.launch(Dispatchers.IO) {
            mutex.withLock {
                try {
                    Log.i(TAG, "loadFile: $uri")

                    stopPlaybackInternal()

                    _state.value = _state.value.copy(
                        currentBitmap = null,
                        isPlaying = false
                    )

                    bitmapCache.clear()

                    manager.close()

                    val context = getApplication<Application>()
                    val success = manager.load(context, uri)

                    if (success) {
                        val info = manager.getInfo()
                        val totalFrames = info?.numFrames ?: 0

                        Log.i(TAG, "Loaded: ${manager.fileName}, frames=$totalFrames")

                        val groups = buildGroupList(info)
                        val frameInfo = manager.getFrameInfo(0)
                        val bitmap = loadBitmap(0)

                        val maxDim = maxOf(frameInfo?.width ?: 1, frameInfo?.height ?: 1)
                        val initialZoom = when {
                            maxDim < 64 -> 4.0f
                            maxDim < 128 -> 2.0f
                            else -> 1.0f
                        }

                        _state.value = SpriteUiState(
                            isLoaded = true,
                            fileName = manager.fileName,
                            spriteInfo = info,
                            totalFrames = totalFrames,
                            currentFrame = 0,
                            currentFrameInfo = frameInfo,
                            currentBitmap = bitmap,
                            isPlaying = false,
                            zoom = initialZoom,
                            offsetX = 0f,
                            offsetY = 0f,
                            showChecker = _state.value.showChecker,
                            showProperties = _state.value.showProperties,
                            showToolbar = _state.value.showToolbar,
                            groups = groups,
                            palette = manager.getPalette(),
                            statusMessage = "Loaded: ${manager.fileName} ($totalFrames frames)",
                            statusTimestamp = System.currentTimeMillis()
                        )
                    } else {
                        Log.e(TAG, "Failed to load sprite")
                        _state.value = _state.value.copy(
                            statusMessage = "Failed to load file",
                            statusTimestamp = System.currentTimeMillis()
                        )
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "loadFile crashed", e)
                    _state.value = _state.value.copy(
                        statusMessage = "Error: ${e.message}",
                        statusTimestamp = System.currentTimeMillis()
                    )
                }
            }
        }
    }

    fun closeFile() {
        viewModelScope.launch(Dispatchers.IO) {
            mutex.withLock {
                stopPlaybackInternal()

                _state.value = _state.value.copy(currentBitmap = null)
                bitmapCache.clear()
                manager.close()

                _state.value = SpriteUiState(
                    statusMessage = "Closed",
                    statusTimestamp = System.currentTimeMillis()
                )
            }
        }
    }

    fun setFrame(index: Int) {
        val s = _state.value
        if (!s.isLoaded || index < 0 || index >= s.totalFrames) return

        viewModelScope.launch(Dispatchers.IO) {
            mutex.withLock {
                if (!manager.isLoaded) return@withLock
                try {
                    val frameInfo = manager.getFrameInfo(index)
                    val bitmap = getOrLoadBitmap(index)

                    _state.value = _state.value.copy(
                        currentFrame = index,
                        currentFrameInfo = frameInfo,
                        currentBitmap = bitmap
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "setFrame failed", e)
                }
            }
        }
    }

    fun nextFrame() {
        val s = _state.value
        if (s.totalFrames <= 1) return
        setFrame((s.currentFrame + 1) % s.totalFrames)
    }

    fun prevFrame() {
        val s = _state.value
        if (s.totalFrames <= 1) return
        setFrame((s.currentFrame - 1 + s.totalFrames) % s.totalFrames)
    }

    fun firstFrame() = setFrame(0)

    fun lastFrame() {
        val s = _state.value
        setFrame(maxOf(0, s.totalFrames - 1))
    }

    fun togglePlayback() {
        val s = _state.value
        if (s.totalFrames <= 1) return

        if (s.isPlaying) {
            stopPlaybackInternal()
            _state.value = _state.value.copy(isPlaying = false)
        } else {
            startPlayback()
        }
    }

    private fun startPlayback() {
        _state.value = _state.value.copy(isPlaying = true)

        playJob?.cancel()
        playJob = viewModelScope.launch(Dispatchers.Default) {
            while (_state.value.isPlaying && _state.value.isLoaded) {
                val s = _state.value
                val interval = s.currentFrameInfo?.interval ?: 0.1f
                val delayMs = ((interval / s.playbackSpeed) * 1000).toLong().coerceAtLeast(16)
                delay(delayMs)

                if (!_state.value.isPlaying || !_state.value.isLoaded) break

                mutex.withLock {
                    if (!manager.isLoaded) return@withLock

                    try {
                        val nextIdx = (_state.value.currentFrame + 1) % _state.value.totalFrames
                        val frameInfo = manager.getFrameInfo(nextIdx)
                        val bitmap = getOrLoadBitmap(nextIdx)

                        _state.value = _state.value.copy(
                            currentFrame = nextIdx,
                            currentFrameInfo = frameInfo,
                            currentBitmap = bitmap
                        )
                    } catch (e: Exception) {
                        Log.e(TAG, "playback frame failed", e)
                        _state.value = _state.value.copy(isPlaying = false)
                    }
                }
            }
        }
    }

    private fun stopPlaybackInternal() {
        playJob?.cancel()
        playJob = null
    }

    fun setPlaybackSpeed(speed: Float) {
        _state.value = _state.value.copy(playbackSpeed = speed.coerceIn(0.1f, 8.0f))
    }

    fun setZoom(zoom: Float) {
        _state.value = _state.value.copy(zoom = zoom.coerceIn(0.25f, 16.0f))
    }

    fun zoomIn() = setZoom(_state.value.zoom * 2.0f)
    fun zoomOut() = setZoom(_state.value.zoom * 0.5f)
    fun resetZoom() {
        _state.value = _state.value.copy(zoom = 1.0f, offsetX = 0f, offsetY = 0f)
    }

    fun setOffset(x: Float, y: Float) {
        _state.value = _state.value.copy(offsetX = x, offsetY = y)
    }

    fun toggleChecker() {
        _state.value = _state.value.copy(showChecker = !_state.value.showChecker)
    }

    fun toggleProperties() {
        _state.value = _state.value.copy(showProperties = !_state.value.showProperties)
    }

    fun toggleToolbar() {
        _state.value = _state.value.copy(showToolbar = !_state.value.showToolbar)
    }

    fun showAbout(show: Boolean) {
        _state.value = _state.value.copy(showAbout = show)
    }

    private fun loadBitmap(index: Int): Bitmap? {
        val bitmap = manager.getFrameBitmap(index) ?: return null
        bitmapCache[index] = bitmap
        return bitmap
    }

    private fun getOrLoadBitmap(index: Int): Bitmap? {
        val cached = bitmapCache[index]
        if (cached != null && !cached.isRecycled) return cached

        val bitmap = manager.getFrameBitmap(index) ?: return null
        bitmapCache[index] = bitmap

        if (bitmapCache.size > 100) {
            val keysToRemove = bitmapCache.keys.take(bitmapCache.size - 80)
            keysToRemove.forEach { bitmapCache.remove(it) }
        }

        return bitmap
    }

    private fun buildGroupList(info: SpriteInfo?): List<GroupDisplayInfo> {
        if (info == null) return emptyList()

        val result = mutableListOf<GroupDisplayInfo>()
        var globalIdx = 0

        for (gi in 0 until info.numGroups) {
            val ginfo = manager.getGroupInfo(gi) ?: continue
            val frames = mutableListOf<FrameDisplayInfo>()

            for (fi in 0 until ginfo.numFrames) {
                val finfo = manager.getFrameInfo(globalIdx)
                if (finfo != null) {
                    frames.add(FrameDisplayInfo(globalIdx, fi, finfo))
                }
                globalIdx++
            }

            result.add(GroupDisplayInfo(gi, ginfo, frames))
        }

        return result
    }
}