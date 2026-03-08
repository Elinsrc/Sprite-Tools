package com.spritetools.core

data class SpriteInfo(
    val version: Int,
    val type: Int,
    val texFormat: Int,
    val facetype: Int,
    val boundsW: Int,
    val boundsH: Int,
    val numFrames: Int,
    val numGroups: Int,
    val paletteColors: Int
) {
    val versionName: String get() = when (version) {
        1 -> "Quake"
        2 -> "Half-Life"
        else -> "Unknown"
    }

    val typeName: String get() = when (type) {
        0 -> "Parallel Upright"
        1 -> "Facing Upright"
        2 -> "Parallel"
        3 -> "Oriented"
        4 -> "Parallel Oriented"
        else -> "Unknown"
    }

    val texFormatName: String get() = when (texFormat) {
        0 -> "Normal"
        1 -> "Additive"
        2 -> "Index Alpha"
        3 -> "Alpha Test"
        else -> "Unknown"
    }

    val faceTypeName: String get() = when (facetype) {
        0 -> "Cull Front"
        1 -> "No Cull"
        else -> "Unknown"
    }

    companion object {
        fun fromArray(arr: IntArray): SpriteInfo? {
            if (arr.size < 9) return null
                return SpriteInfo(
                    arr[0], arr[1], arr[2], arr[3],
                    arr[4], arr[5], arr[6], arr[7], arr[8]
                )
        }
    }
}
