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
    @JvmStatic external fun nativeExportFrameToImage(handle: Long, frameIndex: Int, format: Int): ByteArray?

    @JvmStatic external fun nativeCreateSprFromImages(imageDataArray: Array<ByteArray>, version: Int, type: Int, texFormat: Int, interval: Float): ByteArray?

    @JvmStatic external fun nativeGetLastError(): String?
}