package com.spritetools.ui.components

import android.graphics.Bitmap
import android.graphics.BitmapShader
import android.graphics.Canvas as AndroidCanvas
import android.graphics.Matrix
import android.graphics.Paint as AndroidPaint
import android.graphics.Shader
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.translate
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme
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

    val density = LocalDensity.current
    val checkerBrush = remember(density) {
        val sizePx = with(density) { 10.dp.toPx() }.toInt()
        val bitmap = Bitmap.createBitmap(sizePx * 2, sizePx * 2, Bitmap.Config.ARGB_8888)
        val canvas = AndroidCanvas(bitmap)

        val p1 = AndroidPaint().apply { color = 0xFF181820.toInt() }
        val p2 = AndroidPaint().apply { color = 0xFF282832.toInt() }
        
        canvas.drawRect(0f, 0f, sizePx.toFloat(), sizePx.toFloat(), p1)
        canvas.drawRect(sizePx.toFloat(), sizePx.toFloat(), sizePx * 2f, sizePx * 2f, p1)
        canvas.drawRect(sizePx.toFloat(), 0f, sizePx * 2f, sizePx.toFloat(), p2)
        canvas.drawRect(0f, sizePx.toFloat(), sizePx.toFloat(), sizePx * 2f, p2)
        
        val shader = BitmapShader(bitmap, Shader.TileMode.REPEAT, Shader.TileMode.REPEAT)
        ShaderBrush(shader)
    }

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
                        val newZoom = (currentZoom * zoomChange).coerceIn(0.1f, 32f)
                        val newX = currentOffsetX + pan.x
                        val newY = currentOffsetY + pan.y

                        if (newZoom != currentZoom) onZoomChanged(newZoom)
                        if (newX != currentOffsetX || newY != currentOffsetY) onOffsetChanged(newX, newY)
                    }
                }
            }
    ) {
        if (state.isLoaded) {
            val bitmap = state.currentBitmap
            val imageBitmap = remember(bitmap) { bitmap?.asImageBitmap() }
            
            Canvas(modifier = Modifier.fillMaxSize()) {
                val cx = size.width / 2
                val cy = size.height / 2

                translate(left = cx + state.offsetX, top = cy + state.offsetY) {
                    if (state.showChecker && imageBitmap != null) {
                        val w = imageBitmap.width * state.zoom
                        val h = imageBitmap.height * state.zoom
                        drawRect(
                            brush = checkerBrush,
                            topLeft = Offset(-w / 2, -h / 2),
                            size = Size(w, h)
                        )
                    }

                    imageBitmap?.let {
                        val dstW = (it.width * state.zoom).toInt()
                        val dstH = (it.height * state.zoom).toInt()
                        val dstOffset = androidx.compose.ui.unit.IntOffset(-dstW / 2, -dstH / 2)
                        val dstSize = androidx.compose.ui.unit.IntSize(dstW, dstH)

                        drawImage(
                            image = it,
                            dstOffset = dstOffset,
                            dstSize = dstSize,
                            filterQuality = FilterQuality.None
                        )
                    }
                }
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun SpriteViewportPreview() {
    val bitmap = Bitmap.createBitmap(100, 100, Bitmap.Config.ARGB_8888).apply {
        val canvas = AndroidCanvas(this)
        val paint = AndroidPaint().apply { color = android.graphics.Color.RED }
        canvas.drawRect(0f, 0f, 100f, 100f, paint)
        paint.color = android.graphics.Color.BLUE
        canvas.drawCircle(50f, 50f, 40f, paint)
    }

    SpriteToolsTheme {
        SpriteViewport(
            state = SpriteUiState(
                isLoaded = true,
                currentBitmap = bitmap,
                zoom = 4.0f,
                showChecker = true
            ),
            onOffsetChanged = { _, _ -> },
            onZoomChanged = { _ -> },
            onViewportSizeChanged = { _, _ -> }
        )
    }
}
