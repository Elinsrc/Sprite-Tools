package com.spritetools.core

object SpriteNative {
    init {
        System.loadLibrary("SpriteTools")
    }

    @JvmStatic external fun nativeLoadMemory(data: ByteArray, name: String): Long
    @JvmStatic external fun nativeFree(handle: Long)
    @JvmStatic external fun nativeGetInfo(handle: Long): IntArray?
    @JvmStatic external fun nativeGetFrameInfo(handle: Long, frameIndex: Int): FloatArray?
    @JvmStatic external fun nativeGetFrameARGB(handle: Long, frameIndex: Int): IntArray?
    @JvmStatic external fun nativeGetPalette(handle: Long): ByteArray?
    @JvmStatic external fun nativeGetGroupInfo(handle: Long, groupIndex: Int): IntArray?
    @JvmStatic external fun nativeExportFrame(
        handle: Long, frameIndex: Int, outputPath: String, format: String
    ): Int
    @JvmStatic external fun nativeExportAllFrames(
        handle: Long, outputDir: String, format: String, pattern: String?
    ): Int
}
