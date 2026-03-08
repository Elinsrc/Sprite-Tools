package com.spritetools.ui.components

import android.graphics.Bitmap
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.gestures.rememberTransformableState
import androidx.compose.foundation.gestures.transformable
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun SpriteViewport(
    state: SpriteUiState,
    modifier: Modifier = Modifier,
    onOffsetChanged: (Float, Float) -> Unit,
    onZoomChanged: (Float) -> Unit,
    onOpenFile: () -> Unit
) {
    if (!state.isLoaded) {
        EmptyViewport(modifier = modifier, onOpenFile = onOpenFile)
        return
    }

    val bitmap = state.currentBitmap
    if (bitmap == null || bitmap.isRecycled) {
        Box(
            modifier = modifier
                .fillMaxSize()
                .background(SpriteColors.BgDark),
            contentAlignment = Alignment.Center
        ) {
            Text("Loading...", color = SpriteColors.TextDim)
        }
        return
    }

    val imageBitmap = remember(bitmap) {
        bitmap.asImageBitmap()
    }

    var localZoom by remember { mutableFloatStateOf(state.zoom) }
    var localOffsetX by remember { mutableFloatStateOf(state.offsetX) }
    var localOffsetY by remember { mutableFloatStateOf(state.offsetY) }

    LaunchedEffect(state.zoom) { localZoom = state.zoom }
    LaunchedEffect(state.offsetX) { localOffsetX = state.offsetX }
    LaunchedEffect(state.offsetY) { localOffsetY = state.offsetY }

    val transformState = rememberTransformableState { zoomChange, panChange, _ ->
        val newZoom = (localZoom * zoomChange).coerceIn(0.25f, 16.0f)
        localZoom = newZoom
        onZoomChanged(newZoom)

        val newOffsetX = localOffsetX + panChange.x
        val newOffsetY = localOffsetY + panChange.y
        localOffsetX = newOffsetX
        localOffsetY = newOffsetY
        onOffsetChanged(newOffsetX, newOffsetY)
    }

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(SpriteColors.BgDark)
            .transformable(state = transformState)
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val canvasW = size.width
            val canvasH = size.height

            val imgW = bitmap.width * localZoom
            val imgH = bitmap.height * localZoom

            val ox = (canvasW - imgW) / 2f + localOffsetX
            val oy = (canvasH - imgH) / 2f + localOffsetY

            if (state.showChecker) {
                val cellSize = (12f * localZoom.coerceIn(0.5f, 2f))
                drawChecker(
                    topLeft = Offset(ox, oy),
                    imgSize = Size(imgW, imgH),
                    canvasSize = size,
                    cellSize = cellSize
                )
            }

            drawImage(
                image = imageBitmap,
                dstOffset = androidx.compose.ui.unit.IntOffset(ox.toInt(), oy.toInt()),
                dstSize = androidx.compose.ui.unit.IntSize(imgW.toInt(), imgH.toInt()),
                filterQuality = FilterQuality.None
            )

            drawRect(
                color = SpriteColors.SpriteRect,
                topLeft = Offset(ox, oy),
                size = Size(imgW, imgH),
                style = Stroke(1f)
            )
        }
    }
}

@Composable
private fun EmptyViewport(
    modifier: Modifier,
    onOpenFile: () -> Unit
) {
    Box(
        modifier = modifier
            .fillMaxSize()
            .background(SpriteColors.BgDark)
            .pointerInput(Unit) {
                detectTapGestures(
                    onDoubleTap = { onOpenFile() }
                )
            },
        contentAlignment = Alignment.Center
    ) {
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text(
                "Sprite-Tools",
                fontSize = 28.sp,
                color = SpriteColors.Accent.copy(alpha = 0.8f)
            )
            Spacer(Modifier.height(12.dp))
            Text(
                "Open .spr file or double-tap",
                fontSize = 14.sp,
                color = SpriteColors.TextDim
            )
        }
    }
}

private fun DrawScope.drawChecker(
    topLeft: Offset,
    imgSize: Size,
    canvasSize: Size,
    cellSize: Float
) {
    val visLeft = maxOf(topLeft.x, 0f)
    val visTop = maxOf(topLeft.y, 0f)
    val visRight = minOf(topLeft.x + imgSize.width, canvasSize.width)
    val visBottom = minOf(topLeft.y + imgSize.height, canvasSize.height)

    if (visLeft >= visRight || visTop >= visBottom) return

    val startCol = ((visLeft - topLeft.x) / cellSize).toInt()
    val startRow = ((visTop - topLeft.y) / cellSize).toInt()
    val endCol = ((visRight - topLeft.x) / cellSize).toInt() + 1
    val endRow = ((visBottom - topLeft.y) / cellSize).toInt() + 1

    for (row in startRow..endRow) {
        for (col in startCol..endCol) {
            val cx = topLeft.x + col * cellSize
            val cy = topLeft.y + row * cellSize

            val drawLeft = maxOf(cx, visLeft)
            val drawTop = maxOf(cy, visTop)
            val drawRight = minOf(cx + cellSize, visRight)
            val drawBottom = minOf(cy + cellSize, visBottom)

            if (drawLeft < drawRight && drawTop < drawBottom) {
                drawRect(
                    color = if ((col + row) % 2 == 0) SpriteColors.CheckerA else SpriteColors.CheckerB,
                    topLeft = Offset(drawLeft, drawTop),
                    size = Size(drawRight - drawLeft, drawBottom - drawTop)
                )
            }
        }
    }
}