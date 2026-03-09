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
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import java.io.File
import kotlin.coroutines.coroutineContext

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

    val showExportDialog: Boolean = false,
    val showImportDialog: Boolean = false,

    val showProgress: Boolean = false,
    val progressTitle: String = "",
    val progressStatus: String = "",
    val progressValue: Float = 0f,
    val progressDone: Boolean = false,
    val progressSuccess: Boolean = false,
    val progressResult: String = "",

    val pendingLoadUri: Uri? = null
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
    private val converter = SpriteConverter()
    private val mutex = Mutex()

    private val _state = MutableStateFlow(SpriteUiState())
    val state: StateFlow<SpriteUiState> = _state.asStateFlow()

    private var playJob: Job? = null
    private var convertJob: Job? = null

    private val bitmapCache = mutableMapOf<Int, Bitmap>()

    private var viewportWidth = 0
    private var viewportHeight = 0

    fun onViewportSizeChanged(w: Int, h: Int) {
        viewportWidth = w
        viewportHeight = h
    }

    override fun onCleared() {
        super.onCleared()
        playJob?.cancel()
        playJob = null
        convertJob?.cancel()
        convertJob = null
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

                        val imgW = frameInfo?.width ?: 1
                        val imgH = frameInfo?.height ?: 1
                        var initialZoom = 1.0f

                        if (viewportWidth > 0 && viewportHeight > 0) {
                            val padding = 0.9f
                            val scaleX = (viewportWidth * padding) / imgW
                            val scaleY = (viewportHeight * padding) / imgH
                            val fitScale = minOf(scaleX, scaleY)

                            initialZoom = if (fitScale < 1.0f) {
                                fitScale
                            } else {
                                when {
                                    imgW < 64 && imgH < 64 -> 4.0f
                                    imgW < 128 && imgH < 128 -> 2.0f
                                    else -> 1.0f
                                }
                            }
                        } else {
                            initialZoom = when {
                                imgW < 64 -> 4.0f
                                imgW < 128 -> 2.0f
                                else -> 1.0f
                            }
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
        val z = zoom.coerceIn(0.1f, 16.0f)
        if (_state.value.zoom != z) {
            _state.value = _state.value.copy(zoom = z)
        }
    }

    fun zoomIn() = setZoom(_state.value.zoom * 2.0f)
    fun zoomOut() = setZoom(_state.value.zoom * 0.5f)
    fun resetZoom() {
        _state.value = _state.value.copy(zoom = 1.0f, offsetX = 0f, offsetY = 0f)
    }

    fun setOffset(x: Float, y: Float) {
        // Просто обновляем стейт, не сбрасывая ничего другого
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

    fun showExportDialog(show: Boolean) {
        _state.value = _state.value.copy(showExportDialog = show)
    }

    fun showImportDialog(show: Boolean) {
        _state.value = _state.value.copy(showImportDialog = show)
    }

    fun dismissProgress() {
        val pendingUri = _state.value.pendingLoadUri

        _state.value = _state.value.copy(
            showProgress = false,
            progressDone = false,
            pendingLoadUri = null
        )

        if (pendingUri != null) {
            loadFile(pendingUri)
        }
    }

    fun exportAllFrames(
        outputDir: File,
        baseName: String,
        format: ImageExportFormat
    ) {
        val s = _state.value
        if (!s.isLoaded) return

        val handle = manager.nativeHandle
        val totalFrames = s.totalFrames

        _state.value = _state.value.copy(
            showExportDialog = false,
            showProgress = true,
            progressTitle = "Exporting Frames",
            progressStatus = "Starting...",
            progressValue = 0f,
            progressDone = false
        )

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            outputDir.mkdirs()
            val exported = mutableListOf<String>()

            for (i in 0 until totalFrames) {
                if (!coroutineContext[Job]!!.isActive) {
                    updateProgress("Cancelled", 0f, true, false,
                        "Exported ${exported.size} file(s)")
                    return@launch
                }

                updateProgress(
                    "Exporting frame ${i + 1} / $totalFrames...",
                    (i + 1).toFloat() / totalFrames,
                    false, false, ""
                )

                val name = if (totalFrames == 1)
                    "$baseName${format.ext}"
                else
                    "${baseName}_%03d${format.ext}".format(i)

                val file = File(outputDir, name)
                val result = converter.exportFrameToFile(
                    handle, i, file, format)

                if (!result.success) {
                    updateProgress(
                        "Failed", 0f, true, false,
                        result.error ?: "Export failed at frame $i"
                    )
                    return@launch
                }

                exported.addAll(result.files)
            }

            updateProgress(
                "Done!", 1f, true, true,
                "Saved to:\n${outputDir.absolutePath}"
            )
        }
    }

    fun exportCurrentFrame(
        outputFile: File,
        format: ImageExportFormat
    ) {
        val s = _state.value
        if (!s.isLoaded) return

        _state.value = _state.value.copy(
            showExportDialog = false,
            showProgress = true,
            progressTitle = "Exporting Frame",
            progressStatus = "Saving...",
            progressValue = 0.5f,
            progressDone = false
        )

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            val result = converter.exportFrameToFile(
                manager.nativeHandle, _state.value.currentFrame,
                outputFile, format)

            if (result.success) {
                updateProgress("Done!", 1f, true, true,
                    "Saved: ${outputFile.name}")
            } else {
                updateProgress("Failed", 0f, true, false,
                    result.error ?: "Export failed")
            }
        }
    }

    fun createSprFromUris(
        uris: List<Uri>,
        outputFile: File,
        version: Int,
        type: SprType,
        texFormat: SprTexFormat,
        interval: Float
    ) {
        _state.value = _state.value.copy(
            showImportDialog = false,
            showProgress = true,
            progressTitle = "Creating Sprite",
            progressStatus = "Loading images...",
            progressValue = 0f,
            progressDone = false,
            pendingLoadUri = null
        )

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            val context = getApplication<Application>()
            val total = uris.size
            val imageData = mutableListOf<ByteArray>()

            for ((i, uri) in uris.withIndex()) {
                if (!coroutineContext[Job]!!.isActive) {
                    updateProgress("Cancelled", 0f, true, false, "")
                    return@launch
                }

                updateProgress(
                    "Loading image ${i + 1} / $total...",
                    (i + 1).toFloat() / (total * 2),
                    false, false, ""
                )

                try {
                    val stream = context.contentResolver.openInputStream(uri)
                    if (stream == null) {
                        updateProgress("Failed", 0f, true, false,
                            "Cannot open image ${i + 1}")
                        return@launch
                    }
                    imageData.add(stream.readBytes())
                    stream.close()
                } catch (e: Exception) {
                    updateProgress("Failed", 0f, true, false,
                        "Error reading image: ${e.message}")
                    return@launch
                }
            }

            if (!coroutineContext[Job]!!.isActive) {
                updateProgress("Cancelled", 0f, true, false, "")
                return@launch
            }

            updateProgress("Building sprite...", 0.6f, false, false, "")

            val result = converter.createFromImageData(
                imageData, version, type, texFormat, interval)

            if (!result.success || result.data == null) {
                updateProgress("Failed", 0f, true, false,
                    result.error ?: "Conversion failed")
                return@launch
            }

            updateProgress("Saving...", 0.9f, false, false, "")

            try {
                outputFile.parentFile?.mkdirs()
                outputFile.writeBytes(result.data)
            } catch (e: Exception) {
                updateProgress("Failed", 0f, true, false,
                    "Save error: ${e.message}")
                return@launch
            }

            updateProgress("Done!", 1f, true, true,
                "Created: ${outputFile.name}\n" +
                        "Path: ${outputFile.parent}\n" +
                        "$total frame(s), ${result.data.size} bytes")

            _state.value = _state.value.copy(pendingLoadUri = Uri.fromFile(outputFile))
        }
    }

    fun cancelConvert() {
        convertJob?.cancel()
        convertJob = null
        _state.value = _state.value.copy(
            showProgress = false,
            progressDone = false
        )
    }

    private fun updateProgress(
        status: String,
        value: Float,
        done: Boolean,
        success: Boolean,
        result: String
    ) {
        _state.value = _state.value.copy(
            progressStatus = status,
            progressValue = value,
            progressDone = done,
            progressSuccess = success,
            progressResult = result
        )
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