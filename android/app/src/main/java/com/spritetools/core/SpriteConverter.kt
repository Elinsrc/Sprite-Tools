package com.spritetools.core

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import java.io.ByteArrayOutputStream
import java.io.File

enum class ImageExportFormat(val code: Int, val ext: String, val label: String) {
    PNG(0, ".png", "PNG (with alpha)"),
    BMP(1, ".bmp", "BMP (no alpha)")
}

enum class SprType(val code: Int, val label: String) {
    PARALLEL_UPRIGHT(0, "Parallel Upright"),
    FACING_UPRIGHT(1, "Facing Upright"),
    PARALLEL(2, "Parallel"),
    ORIENTED(3, "Oriented"),
    PARALLEL_ORIENTED(4, "Parallel Oriented")
}

enum class SprTexFormat(val code: Int, val label: String) {
    NORMAL(0, "Normal"),
    ADDITIVE(1, "Additive"),
    INDEX_ALPHA(2, "Index Alpha"),
    ALPHA_TEST(3, "Alpha Test")
}

data class ConvertResult(
    val success: Boolean,
    val error: String? = null,
    val data: ByteArray? = null,
    val files: List<String> = emptyList()
)

class SpriteConverter {

    fun exportFrame(
        handle: Long,
        frameIndex: Int,
        format: ImageExportFormat = ImageExportFormat.PNG
    ): ConvertResult {
        val data = SpriteNative.nativeExportFrameToImage(
            handle, frameIndex, format.code)

        return if (data != null)
            ConvertResult(success = true, data = data)
        else
            ConvertResult(success = false,
                error = SpriteNative.nativeGetLastError() ?: "Export failed")
    }

    fun exportFrameToFile(
        handle: Long,
        frameIndex: Int,
        outputFile: File,
        format: ImageExportFormat = ImageExportFormat.PNG
    ): ConvertResult {
        val result = exportFrame(handle, frameIndex, format)
        if (!result.success || result.data == null) return result

        return try {
            outputFile.parentFile?.mkdirs()
            outputFile.writeBytes(result.data)
            ConvertResult(success = true,
                files = listOf(outputFile.absolutePath))
        } catch (e: Exception) {
            ConvertResult(success = false, error = e.message)
        }
    }

    fun exportFrameAsBitmap(handle: Long, frameIndex: Int): Bitmap? {
        val r = exportFrame(handle, frameIndex, ImageExportFormat.PNG)
        if (!r.success || r.data == null) return null
        return BitmapFactory.decodeByteArray(r.data, 0, r.data.size)
    }

    fun createFromImageData(
        images: List<ByteArray>,
        version: Int = 2,
        type: SprType = SprType.PARALLEL,
        texFormat: SprTexFormat = SprTexFormat.NORMAL,
        interval: Float = 0.1f
    ): ConvertResult {
        if (images.isEmpty())
            return ConvertResult(false, "No images")

        val spr = SpriteNative.nativeCreateSprFromImages(
            images.toTypedArray(),
            version, type.code, texFormat.code, interval)

        return if (spr != null)
            ConvertResult(success = true, data = spr)
        else
            ConvertResult(success = false,
                error = SpriteNative.nativeGetLastError() ?: "Create failed")
    }

    fun createFromUris(
        context: Context,
        uris: List<Uri>,
        version: Int = 2,
        type: SprType = SprType.PARALLEL,
        texFormat: SprTexFormat = SprTexFormat.NORMAL,
        interval: Float = 0.1f
    ): ConvertResult {
        val data = mutableListOf<ByteArray>()
        for (uri in uris) {
            try {
                val s = context.contentResolver.openInputStream(uri)
                    ?: return ConvertResult(false, "Cannot open: $uri")
                data.add(s.readBytes())
                s.close()
            } catch (e: Exception) {
                return ConvertResult(false, "Read error: ${e.message}")
            }
        }
        return createFromImageData(data, version, type, texFormat, interval)
    }

    fun createFromBitmaps(
        bitmaps: List<Bitmap>,
        version: Int = 2,
        type: SprType = SprType.PARALLEL,
        texFormat: SprTexFormat = SprTexFormat.NORMAL,
        interval: Float = 0.1f
    ): ConvertResult {
        val data = bitmaps.map { bmp ->
            val out = ByteArrayOutputStream()
            bmp.compress(Bitmap.CompressFormat.PNG, 100, out)
            out.toByteArray()
        }
        return createFromImageData(data, version, type, texFormat, interval)
    }

    fun createToFile(
        images: List<ByteArray>,
        outputFile: File,
        version: Int = 2,
        type: SprType = SprType.PARALLEL,
        texFormat: SprTexFormat = SprTexFormat.NORMAL,
        interval: Float = 0.1f
    ): ConvertResult {
        val r = createFromImageData(images, version, type, texFormat, interval)
        if (!r.success || r.data == null) return r

        return try {
            outputFile.parentFile?.mkdirs()
            outputFile.writeBytes(r.data)
            ConvertResult(success = true,
                files = listOf(outputFile.absolutePath))
        } catch (e: Exception) {
            ConvertResult(false, e.message)
        }
    }
}