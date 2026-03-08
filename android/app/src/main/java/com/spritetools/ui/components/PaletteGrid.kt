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
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors

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
