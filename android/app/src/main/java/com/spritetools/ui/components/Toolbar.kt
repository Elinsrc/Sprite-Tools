package com.spritetools.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun SpriteToolbar(
    state: SpriteUiState,
    onOpenFile: () -> Unit,
    onFirstFrame: () -> Unit,
    onPrevFrame: () -> Unit,
    onTogglePlay: () -> Unit,
    onNextFrame: () -> Unit,
    onLastFrame: () -> Unit,
    onSpeedChanged: (Float) -> Unit,
    onZoomIn: () -> Unit,
    onZoomOut: () -> Unit,
    onResetZoom: () -> Unit,
    onZoomChanged: (Float) -> Unit,
    onFrameChanged: (Int) -> Unit
) {
    val multi = state.totalFrames > 1

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .background(SpriteColors.ToolbarBg)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(40.dp)
                .padding(horizontal = 2.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            TinyBtn(Icons.Default.SkipPrevious, "First", multi) { onFirstFrame() }
            TinyBtn(Icons.Default.NavigateBefore, "Prev", multi) { onPrevFrame() }
            TinyBtn(
                icon = if (state.isPlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                desc = "Play",
                enabled = multi,
                highlighted = state.isPlaying
            ) { onTogglePlay() }
            TinyBtn(Icons.Default.NavigateNext, "Next", multi) { onNextFrame() }
            TinyBtn(Icons.Default.SkipNext, "Last", multi) { onLastFrame() }

            if (multi) {
                Text(
                    "${state.playbackSpeed.fmt(1)}x",
                    fontSize = 10.sp,
                    color = SpriteColors.TextDim,
                    modifier = Modifier.padding(start = 4.dp)
                )
                Slider(
                    value = state.playbackSpeed,
                    onValueChange = onSpeedChanged,
                    valueRange = 0.1f..8.0f,
                    modifier = Modifier.width(50.dp),
                    colors = SliderDefaults.colors(
                        thumbColor = SpriteColors.Accent,
                        activeTrackColor = SpriteColors.AccentDim
                    )
                )
            }

            Spacer(Modifier.weight(1f))

            TinyBtn(Icons.Default.ZoomOut, "Zoom-") { onZoomOut() }
            TinyBtn(Icons.Default.ZoomIn, "Zoom+") { onZoomIn() }
            TinyBtn(Icons.Default.FilterCenterFocus, "1:1") { onResetZoom() }
        }

        if (multi) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(32.dp)
                    .padding(horizontal = 8.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    "${state.currentFrame + 1}/${state.totalFrames}",
                    fontSize = 11.sp,
                    color = SpriteColors.TextDim,
                    modifier = Modifier.width(48.dp)
                )

                Slider(
                    value = state.currentFrame.toFloat(),
                    onValueChange = { onFrameChanged(it.toInt()) },
                    valueRange = 0f..(state.totalFrames - 1).toFloat(),
                    steps = maxOf(0, state.totalFrames - 2),
                    modifier = Modifier.weight(1f),
                    colors = SliderDefaults.colors(
                        thumbColor = SpriteColors.Accent,
                        activeTrackColor = SpriteColors.AccentDim
                    )
                )
            }
        }
    }
}

@Composable
private fun TinyBtn(
    icon: ImageVector,
    desc: String,
    enabled: Boolean = true,
    highlighted: Boolean = false,
    onClick: () -> Unit
) {
    IconButton(
        onClick = onClick,
        enabled = enabled,
        modifier = Modifier.size(34.dp)
    ) {
        Icon(
            icon,
            contentDescription = desc,
            modifier = Modifier.size(18.dp),
            tint = when {
                !enabled -> SpriteColors.TextDim.copy(alpha = 0.3f)
                highlighted -> SpriteColors.AccentLit
                else -> SpriteColors.TextPrimary
            }
        )
    }
}

private fun Float.fmt(d: Int): String = "%.${d}f".format(this)