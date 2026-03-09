package com.spritetools.ui.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.translate
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun SpriteViewport(
    state: SpriteUiState,
    modifier: Modifier = Modifier,
    onOffsetChanged: (Float, Float) -> Unit,
    onZoomChanged: (Float) -> Unit,
    onViewportSizeChanged: (Int, Int) -> Unit
) {

    val currentZoom by rememberUpdatedState(state.zoom)
    val currentOffsetX by rememberUpdatedState(state.offsetX)
    val currentOffsetY by rememberUpdatedState(state.offsetY)

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(SpriteColors.BgDark)
            .clipToBounds()
            .onSizeChanged { size ->
                onViewportSizeChanged(size.width, size.height)
            }
            .pointerInput(state.isLoaded) {
                if (state.isLoaded) {
                    detectTransformGestures { _, pan, zoomChange, _ ->

                        val newZoom = (currentZoom * zoomChange).coerceIn(0.1f, 16f)

                        val newX = currentOffsetX + pan.x
                        val newY = currentOffsetY + pan.y

                        if (newZoom != currentZoom) {
                            onZoomChanged(newZoom)
                        }

                        if (newX != currentOffsetX || newY != currentOffsetY) {
                            onOffsetChanged(newX, newY)
                        }
                    }
                }
            }
    ) {
        if (!state.isLoaded) {
        } else {
            val bitmap = state.currentBitmap?.asImageBitmap()
            val filter = FilterQuality.None

            Canvas(modifier = Modifier.fillMaxSize()) {
                val cx = size.width / 2
                val cy = size.height / 2

                translate(left = cx + state.offsetX, top = cy + state.offsetY) {

                    if (state.showChecker && bitmap != null) {
                        val w = bitmap.width * state.zoom
                        val h = bitmap.height * state.zoom
                        drawRect(
                            color = Color(0xFF2A2A2E),
                            topLeft = Offset(-w / 2, -h / 2),
                            size = androidx.compose.ui.geometry.Size(w, h)
                        )
                    }

                    if (bitmap != null) {
                        val dstW = (bitmap.width * state.zoom).toInt()
                        val dstH = (bitmap.height * state.zoom).toInt()

                        val dstOffset = androidx.compose.ui.unit.IntOffset(
                            -dstW / 2,
                            -dstH / 2
                        )
                        val dstSize = androidx.compose.ui.unit.IntSize(dstW, dstH)

                        drawImage(
                            image = bitmap,
                            dstOffset = dstOffset,
                            dstSize = dstSize,
                            filterQuality = filter
                        )
                    }
                }
            }
        }
    }
}