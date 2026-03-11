package com.spritetools.ui.components

import androidx.compose.foundation.*
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.R
import com.spritetools.core.FrameInfo
import com.spritetools.core.GroupInfo
import com.spritetools.core.SpriteInfo
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme
import com.spritetools.viewmodel.FrameDisplayInfo
import com.spritetools.viewmodel.GroupDisplayInfo
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun PropertiesPanel(
    state: SpriteUiState,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current

    Column(
        modifier = modifier
            .fillMaxWidth()
            .verticalScroll(scrollState)
            .padding(horizontal = 16.dp, vertical = 8.dp)
    ) {
        SectionHeader(stringResource(R.string.section_file))
        Text(state.fileName, fontSize = 13.sp, color = SpriteColors.TextPrimary)

        val info = state.spriteInfo
        if (info != null) {
            SectionHeader(stringResource(R.string.section_sprite))
            PropRow(stringResource(R.string.prop_version), info.getVersionName(context))
            PropRow(stringResource(R.string.prop_type), info.getTypeName(context))
            PropRow(stringResource(R.string.prop_render), info.getTexFormatName(context))
            PropRow(stringResource(R.string.prop_cull), info.getFaceTypeName(context))
            PropRow(stringResource(R.string.prop_bounds), "${info.boundsW} × ${info.boundsH}")
            PropRow(stringResource(R.string.prop_frames), "${state.totalFrames}")
        }

        val frame = state.currentFrameInfo
        if (frame != null) {
            SectionHeader(stringResource(R.string.section_frame))
            PropRow(stringResource(R.string.prop_index), "${state.currentFrame + 1} / ${state.totalFrames}")
            PropRow(stringResource(R.string.prop_size), "${frame.width} × ${frame.height}")
            PropRow(stringResource(R.string.prop_origin), "${frame.originX}, ${frame.originY}")
            PropRow(stringResource(R.string.prop_interval), stringResource(R.string.prop_interval_val, frame.interval))

            if (state.isPlaying) {
                Text(
                    stringResource(R.string.prop_playing_indicator),
                    fontSize = 12.sp,
                    color = SpriteColors.Playing
                )
            }
        }

        if (state.groups.isNotEmpty()) {
            SectionHeader(stringResource(R.string.section_groups))
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
                                "  • ${f.info.width}×${f.info.height}  (${f.info.originX},${f.info.originY})  " + 
                                stringResource(R.string.prop_interval_val_short, f.info.interval),
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
        if (!palette.isNullOrEmpty()) {
            SectionHeader(stringResource(R.string.section_palette))
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

@Preview(showBackground = true, backgroundColor = 0xFF1E1E1E)
@Composable
fun PropertiesPanelPreview() {
    val sampleState = SpriteUiState(
        isLoaded = true,
        fileName = "sample.spr",
        spriteInfo = SpriteInfo(
            version = 2,
            type = 0,
            texFormat = 0,
            facetype = 1,
            boundsW = 64,
            boundsH = 64,
            numFrames = 10,
            numGroups = 1,
            paletteColors = 256
        ),
        totalFrames = 10,
        currentFrame = 0,
        currentFrameInfo = FrameInfo(
            width = 64,
            height = 64,
            originX = 32,
            originY = 32,
            interval = 0.1f,
            groupIndex = 0,
            frameInGroup = 0
        ),
        isPlaying = true,
        groups = listOf(
            GroupDisplayInfo(
                index = 0,
                info = GroupInfo(type = 0, numFrames = 10),
                frames = List(10) { i ->
                    FrameDisplayInfo(
                        globalIndex = i,
                        localIndex = i,
                        info = FrameInfo(
                            width = 64,
                            height = 64,
                            originX = 32,
                            originY = 32,
                            interval = 0.1f,
                            groupIndex = 0,
                            frameInGroup = i
                        )
                    )
                }
            )
        ),
        palette = List(256) { i -> (0xFF000000.toInt() or (i shl 16) or (i shl 8) or i) }
    )

    SpriteToolsTheme {
        Surface(
            modifier = Modifier.width(300.dp),
            color = MaterialTheme.colorScheme.surface
        ) {
            PropertiesPanel(state = sampleState)
        }
    }
}
