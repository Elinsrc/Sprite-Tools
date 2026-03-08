package com.spritetools.core

data class FrameInfo(
    val width: Int,
    val height: Int,
    val originX: Int,
    val originY: Int,
    val interval: Float,
    val groupIndex: Int,
    val frameInGroup: Int
) {
    companion object {
        fun fromArray(arr: FloatArray): FrameInfo? {
            if (arr.size < 7) return null
                return FrameInfo(
                    arr[0].toInt(), arr[1].toInt(),
                                 arr[2].toInt(), arr[3].toInt(),
                                 arr[4],
                                 arr[5].toInt(), arr[6].toInt()
                )
        }
    }
}
