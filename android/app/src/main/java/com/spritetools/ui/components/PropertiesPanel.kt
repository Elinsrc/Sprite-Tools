package com.spritetools.ui.components

import androidx.compose.foundation.*
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun PropertiesPanel(
    state: SpriteUiState,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    Column(
        modifier = modifier
            .fillMaxWidth()
            .verticalScroll(scrollState)
            .padding(horizontal = 16.dp, vertical = 8.dp)
    ) {
        SectionHeader("File")
        Text(state.fileName, fontSize = 13.sp, color = SpriteColors.TextPrimary)

        val info = state.spriteInfo
        if (info != null) {
            SectionHeader("Sprite")
            PropRow("Version", "${info.version} (${info.versionName})")
            PropRow("Type", info.typeName)
            PropRow("Render", info.texFormatName)
            PropRow("Cull", info.faceTypeName)
            PropRow("Bounds", "${info.boundsW} × ${info.boundsH}")
            PropRow("Frames", "${state.totalFrames}")
        }

        val frame = state.currentFrameInfo
        if (frame != null) {
            SectionHeader("Frame")
            PropRow("Index", "${state.currentFrame + 1} / ${state.totalFrames}")
            PropRow("Size", "${frame.width} × ${frame.height}")
            PropRow("Origin", "${frame.originX}, ${frame.originY}")
            PropRow("Interval", "%.4f s".format(frame.interval))

            if (state.isPlaying) {
                Text(
                    " ▶ Playing",
                    fontSize = 12.sp,
                    color = SpriteColors.Playing
                )
            }
        }

        if (state.groups.isNotEmpty()) {
            SectionHeader("Groups")
            state.groups.forEach { group ->
                var expanded by remember { mutableStateOf(false) }
                Column {
                    TextButton(
                        onClick = { expanded = !expanded },
                        modifier = Modifier.fillMaxWidth(),
                        contentPadding = PaddingValues(horizontal = 0.dp, vertical = 4.dp)
                    ) {
                        Text(
                            "${group.info.typeName} #${group.index} (${group.frames.size})",
                            fontSize = 12.sp,
                            color = SpriteColors.TextPrimary,
                            modifier = Modifier.weight(1f)
                        )
                        Text(
                            if (expanded) "▼" else "▶",
                            fontSize = 12.sp,
                            color = SpriteColors.TextDim
                        )
                    }
                    if (expanded) {
                        group.frames.forEach { f ->
                            Text(
                                "  • ${f.info.width}×${f.info.height}  (${f.info.originX},${f.info.originY})  %.3fs".format(f.info.interval),
                                fontSize = 11.sp,
                                color = SpriteColors.TextDim,
                                modifier = Modifier.padding(start = 16.dp, bottom = 2.dp)
                            )
                        }
                    }
                }
            }
        }

        val palette = state.palette
        if (palette != null && palette.isNotEmpty()) {
            SectionHeader("Palette")
            PaletteGrid(colors = palette)
        }

        Spacer(Modifier.height(32.dp))
    }
}

@Composable
private fun SectionHeader(title: String) {
    Spacer(Modifier.height(12.dp))
    Row(
        modifier = Modifier.fillMaxWidth(),
        verticalAlignment = androidx.compose.ui.Alignment.CenterVertically
    ) {
        Divider(modifier = Modifier.width(6.dp), color = SpriteColors.Border)
        Spacer(Modifier.width(6.dp))
        Text(title, fontSize = 13.sp, color = SpriteColors.TextSection)
        Spacer(Modifier.width(6.dp))
        Divider(modifier = Modifier.weight(1f), color = SpriteColors.Border)
    }
    Spacer(Modifier.height(6.dp))
}

@Composable
private fun PropRow(label: String, value: String) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 1.dp)
    ) {
        Text(label, fontSize = 12.sp, color = SpriteColors.TextDim, modifier = Modifier.width(90.dp))
        Text(value, fontSize = 12.sp, color = SpriteColors.TextPrimary)
    }
}