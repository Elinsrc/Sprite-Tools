package com.spritetools.ui.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme

@Composable
fun PaletteGrid(
    colors: List<Int>,
    modifier: Modifier = Modifier,
    columns: Int = 16
) {
    val cellSize = 10f
    val gap = 1f
    val rows = (colors.size + columns - 1) / columns

    var selectedIndex by remember { mutableStateOf(-1) }

    Column(modifier = modifier) {
        Canvas(
            modifier = Modifier
            .width(((cellSize + gap) * columns).dp)
            .height(((cellSize + gap) * rows).dp)
            .pointerInput(colors) {
                detectTapGestures { offset ->
                    val col = (offset.x / (cellSize + gap).dp.toPx()).toInt()
                    val row = (offset.y / (cellSize + gap).dp.toPx()).toInt()
                    val idx = row * columns + col
                    selectedIndex = if (idx in colors.indices) idx else -1
                }
            }
        ) {
            val cellPx = (cellSize + gap).dp.toPx()
            val sizePx = cellSize.dp.toPx()

            colors.forEachIndexed { i, argb ->
                val col = i % columns
                val row = i / columns
                val x = col * cellPx
                val y = row * cellPx

                drawRect(
                    color = Color(argb),
                         topLeft = Offset(x, y),
                         size = Size(sizePx, sizePx)
                )

                if (i == selectedIndex) {
                    drawRect(
                        color = Color.Yellow,
                        topLeft = Offset(x, y),
                             size = Size(sizePx, sizePx),
                             style = Stroke(2f)
                    )
                }
            }
        }

        if (selectedIndex in colors.indices) {
            val c = colors[selectedIndex]
            val r = (c shr 16) and 0xFF
            val g = (c shr 8) and 0xFF
            val b = c and 0xFF

            Spacer(Modifier.height(4.dp))
            Text(
                "#$selectedIndex  RGB($r, $g, $b)  #${"%02X%02X%02X".format(r, g, b)}",
                 fontSize = 11.sp,
                 color = SpriteColors.TextDim
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun PaletteGridPreview() {
    val sampleColors = listOf(
        0xFF000000.toInt(), 0xFFFFFFFF.toInt(), 0xFFFF0000.toInt(), 0xFF00FF00.toInt(),
        0xFF0000FF.toInt(), 0xFFFFFF00.toInt(), 0xFFFF00FF.toInt(), 0xFF00FFFF.toInt(),
        0xFF808080.toInt(), 0xFFC0C0C0.toInt(), 0xFF800000.toInt(), 0xFF808000.toInt(),
        0xFF008000.toInt(), 0xFF800080.toInt(), 0xFF008080.toInt(), 0xFF000080.toInt()
    )
    SpriteToolsTheme {
        PaletteGrid(colors = sampleColors)
    }
}
