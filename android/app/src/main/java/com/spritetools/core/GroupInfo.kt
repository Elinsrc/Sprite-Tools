package com.spritetools.core

data class GroupInfo(
    val type: Int,
    val numFrames: Int
) {
    val typeName: String get() = when (type) {
        0 -> "Single"
        1 -> "Group"
        2 -> "Angled"
        else -> "Unknown"
    }

    companion object {
        fun fromArray(arr: IntArray): GroupInfo? {
            if (arr.size < 2) return null
                return GroupInfo(arr[0], arr[1])
        }
    }
}
