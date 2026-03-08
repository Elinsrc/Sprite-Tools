package com.spritetools.core

import android.content.Context
import android.graphics.Bitmap
import android.net.Uri
import android.util.Log
import java.io.Closeable

class SpriteManager : Closeable {
    private var handle: Long = 0

    val isLoaded: Boolean get() = handle != 0L
    var fileName: String = ""
        private set

    companion object {
        private const val TAG = "SpriteManager"
    }

    fun load(context: Context, uri: Uri): Boolean {
        close()

        val data = try {
            context.contentResolver.openInputStream(uri)?.use { stream ->
                stream.readBytes()
            }
        } catch (e: Exception) {
            Log.e(TAG, "Failed to read file from URI: $uri", e)
            null
        }

        if (data == null) {
            Log.e(TAG, "Data is null for URI: $uri")
            return false
        }

        if (data.isEmpty()) {
            Log.e(TAG, "Data is empty for URI: $uri")
            return false
        }

        fileName = getFileName(context, uri)
        Log.i(TAG, "Loading sprite: $fileName (${data.size} bytes)")

        handle = try {
            SpriteNative.nativeLoadMemory(data, fileName)
        } catch (e: Exception) {
            Log.e(TAG, "JNI load failed", e)
            0
        }

        Log.i(TAG, "Load result: handle=$handle")
        return handle != 0L
    }

    fun getInfo(): SpriteInfo? {
        if (handle == 0L) return null
        return try {
            val arr = SpriteNative.nativeGetInfo(handle) ?: return null
            SpriteInfo.fromArray(arr)
        } catch (e: Exception) {
            Log.e(TAG, "getInfo failed", e)
            null
        }
    }

    fun getFrameInfo(index: Int): FrameInfo? {
        if (handle == 0L) return null
        return try {
            val arr = SpriteNative.nativeGetFrameInfo(handle, index) ?: return null
            FrameInfo.fromArray(arr)
        } catch (e: Exception) {
            Log.e(TAG, "getFrameInfo failed", e)
            null
        }
    }

    fun getGroupInfo(index: Int): GroupInfo? {
        if (handle == 0L) return null
        return try {
            val arr = SpriteNative.nativeGetGroupInfo(handle, index) ?: return null
            GroupInfo.fromArray(arr)
        } catch (e: Exception) {
            Log.e(TAG, "getGroupInfo failed", e)
            null
        }
    }

    fun getFrameBitmap(index: Int): Bitmap? {
        if (handle == 0L) return null
        return try {
            val info = getFrameInfo(index) ?: return null
            val argb = SpriteNative.nativeGetFrameARGB(handle, index) ?: return null

            val bitmap = Bitmap.createBitmap(
                info.width, info.height, Bitmap.Config.ARGB_8888
            )
            bitmap.setPixels(argb, 0, info.width, 0, 0, info.width, info.height)
            bitmap
        } catch (e: Exception) {
            Log.e(TAG, "getFrameBitmap failed", e)
            null
        }
    }

    fun getPalette(): List<Int>? {
        if (handle == 0L) return null
        return try {
            val raw = SpriteNative.nativeGetPalette(handle) ?: return null
            val colors = mutableListOf<Int>()
            var i = 0
            while (i + 2 < raw.size) {
                val r = raw[i].toInt() and 0xFF
                val g = raw[i + 1].toInt() and 0xFF
                val b = raw[i + 2].toInt() and 0xFF
                colors.add((0xFF shl 24) or (r shl 16) or (g shl 8) or b)
                i += 3
            }
            colors
        } catch (e: Exception) {
            Log.e(TAG, "getPalette failed", e)
            null
        }
    }

    fun exportFrame(index: Int, outputPath: String, format: String): Boolean {
        if (handle == 0L) return false
        return SpriteNative.nativeExportFrame(handle, index, outputPath, format) == 0
    }

    fun exportAllFrames(outputDir: String, format: String, pattern: String? = null): Boolean {
        if (handle == 0L) return false
        return SpriteNative.nativeExportAllFrames(handle, outputDir, format, pattern) == 0
    }

    override fun close() {
        if (handle != 0L) {
            Log.i(TAG, "Freeing sprite handle")
            SpriteNative.nativeFree(handle)
            handle = 0
            fileName = ""
        }
    }

    private fun getFileName(context: Context, uri: Uri): String {
        try {
            context.contentResolver.query(uri, null, null, null, null)?.use { cursor ->
                val nameIndex = cursor.getColumnIndex(android.provider.OpenableColumns.DISPLAY_NAME)
                if (nameIndex >= 0 && cursor.moveToFirst()) {
                    return cursor.getString(nameIndex)
                }
            }
        } catch (_: Exception) {}

        return uri.lastPathSegment?.substringAfterLast('/') ?: "sprite.spr"
    }
}