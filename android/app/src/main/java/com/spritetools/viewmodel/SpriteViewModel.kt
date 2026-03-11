package com.spritetools.viewmodel

import android.app.Application
import android.graphics.Bitmap
import android.net.Uri
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.spritetools.R
import com.spritetools.core.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import java.io.File

data class SpriteUiState(
    val isLoaded: Boolean = false,
    val fileName: String = "",
    val statusMessage: String = "",
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

    private val bitmapCache = LinkedHashMap<Int, Bitmap>(100, 0.75f, true)

    private var viewportWidth = 0
    private var viewportHeight = 0

    private fun getString(resId: Int, vararg args: Any): String {
        return getApplication<Application>().getString(resId, *args)
    }

    fun onViewportSizeChanged(w: Int, h: Int) {
        viewportWidth = w
        viewportHeight = h
    }

    override fun onCleared() {
        super.onCleared()
        playJob?.cancel()
        convertJob?.cancel()
        bitmapCache.clear()
        manager.close()
    }

    fun loadFile(uri: Uri) {
        viewModelScope.launch(Dispatchers.IO) {
            mutex.withLock {
                try {
                    stopPlaybackInternal()
                    _state.update { it.copy(currentBitmap = null, isPlaying = false) }
                    bitmapCache.clear()
                    manager.close()

                    val context = getApplication<Application>()
                    val success = manager.load(context, uri)

                    if (success) {
                        val info = manager.getInfo()
                        val totalFrames = info?.numFrames ?: 0
                        val groups = buildGroupList(info)
                        val frameInfo = manager.getFrameInfo(0)
                        val bitmap = loadBitmap(0)

                        val imgW = frameInfo?.width ?: 1
                        val imgH = frameInfo?.height ?: 1
                        val initialZoom = calculateInitialZoom(imgW, imgH)

                        _state.update {
                            it.copy(
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
                                groups = groups,
                                palette = manager.getPalette(),
                                statusMessage = getString(R.string.status_loaded, manager.fileName, totalFrames),
                                statusTimestamp = System.currentTimeMillis()
                            )
                        }
                    } else {
                        _state.update {
                            it.copy(
                                statusMessage = getString(R.string.status_failed),
                                statusTimestamp = System.currentTimeMillis()
                            )
                        }
                    }
                } catch (e: Exception) {
                    _state.update {
                        it.copy(
                            statusMessage = getString(R.string.status_error, e.message ?: ""),
                            statusTimestamp = System.currentTimeMillis()
                        )
                    }
                }
            }
        }
    }

    private fun calculateInitialZoom(imgW: Int, imgH: Int): Float {
        if (viewportWidth > 0 && viewportHeight > 0) {
            val padding = 0.9f
            val scaleX = (viewportWidth * padding) / imgW
            val scaleY = (viewportHeight * padding) / imgH
            val fitScale = minOf(scaleX, scaleY)

            return if (fitScale < 1.0f) fitScale
            else when {
                imgW < 64 && imgH < 64 -> 4.0f
                imgW < 128 && imgH < 128 -> 2.0f
                else -> 1.0f
            }
        }
        return when {
            imgW < 64 -> 4.0f
            imgW < 128 -> 2.0f
            else -> 1.0f
        }
    }

    fun closeFile() {
        viewModelScope.launch(Dispatchers.IO) {
            mutex.withLock {
                stopPlaybackInternal()
                _state.update { it.copy(currentBitmap = null) }
                bitmapCache.clear()
                manager.close()

                _state.update {
                    SpriteUiState(
                        statusMessage = getString(R.string.status_closed),
                        statusTimestamp = System.currentTimeMillis(),
                        showChecker = it.showChecker,
                        showToolbar = it.showToolbar
                    )
                }
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

                    _state.update {
                        it.copy(
                            currentFrame = index,
                            currentFrameInfo = frameInfo,
                            currentBitmap = bitmap
                        )
                    }
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
            _state.update { it.copy(isPlaying = false) }
        } else {
            startPlayback()
        }
    }

    private fun startPlayback() {
        _state.update { it.copy(isPlaying = true) }

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

                        _state.update {
                            it.copy(
                                currentFrame = nextIdx,
                                currentFrameInfo = frameInfo,
                                currentBitmap = bitmap
                            )
                        }
                    } catch (e: Exception) {
                        _state.update { it.copy(isPlaying = false) }
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
        _state.update { it.copy(playbackSpeed = speed.coerceIn(0.1f, 8.0f)) }
    }

    fun setZoom(zoom: Float) {
        val z = zoom.coerceIn(0.1f, 16.0f)
        _state.update { it.copy(zoom = z) }
    }

    fun zoomIn() = setZoom(_state.value.zoom * 2.0f)
    fun zoomOut() = setZoom(_state.value.zoom * 0.5f)
    fun resetZoom() {
        _state.update { it.copy(zoom = 1.0f, offsetX = 0f, offsetY = 0f) }
    }

    fun setOffset(x: Float, y: Float) {
        _state.update { it.copy(offsetX = x, offsetY = y) }
    }

    fun toggleChecker() {
        _state.update { it.copy(showChecker = !it.showChecker) }
    }

    fun toggleProperties() {
        _state.update { it.copy(showProperties = !it.showProperties) }
    }

    fun toggleToolbar() {
        _state.update { it.copy(showToolbar = !it.showToolbar) }
    }

    fun showAbout(show: Boolean) {
        _state.update { it.copy(showAbout = show) }
    }

    fun showExportDialog(show: Boolean) {
        _state.update { it.copy(showExportDialog = show) }
    }

    fun showImportDialog(show: Boolean) {
        _state.update { it.copy(showImportDialog = show) }
    }

    fun dismissProgress() {
        val pendingUri = _state.value.pendingLoadUri

        _state.update {
            it.copy(
                showProgress = false,
                progressDone = false,
                pendingLoadUri = null
            )
        }

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

        _state.update {
            it.copy(
                showExportDialog = false,
                showProgress = true,
                progressTitle = getString(R.string.export_title),
                progressStatus = getString(R.string.progress_starting),
                progressValue = 0f,
                progressDone = false
            )
        }

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            outputDir.mkdirs()
            val exported = mutableListOf<String>()

            for (i in 0 until totalFrames) {
                if (!coroutineContext[Job]!!.isActive) {
                    updateProgress(getString(R.string.btn_cancel), 0f, true, false, "")
                    return@launch
                }

                updateProgress(
                    getString(R.string.progress_exporting_frame, i + 1, totalFrames),
                    (i + 1).toFloat() / totalFrames,
                    false, false, ""
                )

                val name = if (totalFrames == 1) "$baseName${format.ext}"
                else "${baseName}_%03d${format.ext}".format(i)

                val file = File(outputDir, name)
                val result = converter.exportFrameToFile(handle, i, file, format)

                if (!result.success) {
                    updateProgress(getString(R.string.progress_failed), 0f, true, false, result.error ?: "")
                    return@launch
                }
                exported.addAll(result.files)
            }

            updateProgress(getString(R.string.btn_ok), 1f, true, true, getString(R.string.progress_saved_to, outputDir.absolutePath))
        }
    }

    fun exportCurrentFrame(outputFile: File, format: ImageExportFormat) {
        val s = _state.value
        if (!s.isLoaded) return

        _state.update {
            it.copy(
                showExportDialog = false,
                showProgress = true,
                progressTitle = getString(R.string.export_title),
                progressStatus = getString(R.string.progress_saving),
                progressValue = 0.5f,
                progressDone = false
            )
        }

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            val result = converter.exportFrameToFile(
                manager.nativeHandle, _state.value.currentFrame,
                outputFile, format)

            if (result.success) {
                updateProgress(getString(R.string.btn_ok), 1f, true, true, getString(R.string.progress_saved_file, outputFile.name))
            } else {
                updateProgress(getString(R.string.progress_failed), 0f, true, false, result.error ?: "")
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
        _state.update {
            it.copy(
                showImportDialog = false,
                showProgress = true,
                progressTitle = getString(R.string.create_spr_title),
                progressStatus = getString(R.string.progress_loading_images),
                progressValue = 0f,
                progressDone = false,
                pendingLoadUri = null
            )
        }

        convertJob?.cancel()
        convertJob = viewModelScope.launch(Dispatchers.IO) {
            var finalFile = outputFile
            if (finalFile.exists()) {
                val dir = finalFile.parentFile
                val name = finalFile.nameWithoutExtension
                val ext = finalFile.extension
                var counter = 1
                while (finalFile.exists()) {
                    finalFile = File(dir, "${name}_$counter.$ext")
                    counter++
                }
            }

            val context = getApplication<Application>()
            val total = uris.size
            val imageData = mutableListOf<ByteArray>()

            for ((i, uri) in uris.withIndex()) {
                if (!coroutineContext[Job]!!.isActive) {
                    updateProgress(getString(R.string.btn_cancel), 0f, true, false, "")
                    return@launch
                }

                updateProgress(getString(R.string.progress_loading_image_n, i + 1, total), (i + 1).toFloat() / (total * 2), false, false, "")

                try {
                    context.contentResolver.openInputStream(uri)?.use { imageData.add(it.readBytes()) }
                        ?: throw Exception("Cannot open stream")
                } catch (e: Exception) {
                    updateProgress(getString(R.string.progress_failed), 0f, true, false, e.message ?: "")
                    return@launch
                }
            }

            updateProgress(getString(R.string.progress_building_sprite), 0.6f, false, false, "")

            val result = converter.createFromImageData(imageData, version, type, texFormat, interval)
            if (!result.success || result.data == null) {
                updateProgress(getString(R.string.progress_failed), 0f, true, false, result.error ?: "")
                return@launch
            }

            updateProgress(getString(R.string.progress_saving), 0.9f, false, false, "")

            try {
                finalFile.parentFile?.mkdirs()
                finalFile.writeBytes(result.data)
            } catch (e: Exception) {
                updateProgress(getString(R.string.progress_failed), 0f, true, false, e.message ?: "")
                return@launch
            }

            updateProgress(getString(R.string.btn_ok), 1f, true, true, getString(R.string.progress_created_summary, finalFile.name, finalFile.parent, total))
            _state.update { it.copy(pendingLoadUri = Uri.fromFile(finalFile)) }
        }
    }

    fun cancelConvert() {
        convertJob?.cancel()
        convertJob = null
        _state.update { it.copy(showProgress = false, progressDone = false) }
    }

    private fun updateProgress(status: String, value: Float, done: Boolean, success: Boolean, result: String) {
        _state.update {
            it.copy(
                progressStatus = status,
                progressValue = value,
                progressDone = done,
                progressSuccess = success,
                progressResult = result
            )
        }
    }

    private fun loadBitmap(index: Int): Bitmap? {
        val bitmap = manager.getFrameBitmap(index) ?: return null
        synchronized(bitmapCache) { bitmapCache[index] = bitmap }
        return bitmap
    }

    private fun getOrLoadBitmap(index: Int): Bitmap? {
        synchronized(bitmapCache) {
            val cached = bitmapCache[index]
            if (cached != null && !cached.isRecycled) return cached
        }

        val bitmap = manager.getFrameBitmap(index) ?: return null
        synchronized(bitmapCache) {
            bitmapCache[index] = bitmap
            if (bitmapCache.size > 100) {
                val it = bitmapCache.entries.iterator()
                if (it.hasNext()) { it.next(); it.remove() }
            }
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
                manager.getFrameInfo(globalIdx)?.let { frames.add(FrameDisplayInfo(globalIdx, fi, it)) }
                globalIdx++
            }
            result.add(GroupDisplayInfo(gi, ginfo, frames))
        }
        return result
    }
}
