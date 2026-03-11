package com.spritetools.core

import android.content.Context
import com.spritetools.R

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
    fun getVersionName(context: Context): String = when (version) {
        1 -> context.getString(R.string.version_quake)
        2 -> context.getString(R.string.version_hl)
        else -> context.getString(R.string.unknown)
    }

    fun getTypeName(context: Context): String = when (type) {
        0 -> context.getString(R.string.type_parallel_upright)
        1 -> context.getString(R.string.type_facing_upright)
        2 -> context.getString(R.string.type_parallel)
        3 -> context.getString(R.string.type_oriented)
        4 -> context.getString(R.string.type_parallel_oriented)
        else -> context.getString(R.string.unknown)
    }

    fun getTexFormatName(context: Context): String = when (texFormat) {
        0 -> context.getString(R.string.tex_normal)
        1 -> context.getString(R.string.tex_additive)
        2 -> context.getString(R.string.tex_index_alpha)
        3 -> context.getString(R.string.tex_alpha_test)
        else -> context.getString(R.string.unknown)
    }

    fun getFaceTypeName(context: Context): String = when (facetype) {
        0 -> context.getString(R.string.cull_front)
        1 -> context.getString(R.string.no_cull)
        else -> context.getString(R.string.unknown)
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
