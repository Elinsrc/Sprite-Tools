package com.spritetools.ui.screens

import androidx.compose.animation.*
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.spritetools.ui.components.*
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    viewModel: SpriteViewModel,
    onOpenFile: () -> Unit
) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    var showMenu by remember { mutableStateOf(false) }


    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = false)

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(SpriteColors.BgDark)
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .statusBarsPadding()
                .navigationBarsPadding()
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(44.dp)
                    .background(SpriteColors.BgMid)
                    .padding(horizontal = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Box {
                    IconButton(onClick = { showMenu = true }) {
                        Icon(
                            Icons.Default.Menu,
                            contentDescription = "Menu",
                            tint = SpriteColors.TextPrimary,
                            modifier = Modifier.size(22.dp)
                        )
                    }
                    DropdownMenu(
                        expanded = showMenu,
                        onDismissRequest = { showMenu = false }
                    ) {
                        DropdownMenuItem(
                            text = { Text("Open") },
                            onClick = { showMenu = false; onOpenFile() },
                            leadingIcon = { Icon(Icons.Default.FolderOpen, null) }
                        )
                        if (state.isLoaded) {
                            DropdownMenuItem(
                                text = { Text("Close") },
                                onClick = { showMenu = false; viewModel.closeFile() },
                                leadingIcon = { Icon(Icons.Default.Close, null) }
                            )
                        }
                        Divider()
                        DropdownMenuItem(
                            text = {
                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    Text("Toolbar")
                                    Spacer(Modifier.weight(1f))
                                    if (state.showToolbar) Icon(
                                        Icons.Default.Check, null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                }
                            },
                            onClick = { viewModel.toggleToolbar() }
                        )
                        DropdownMenuItem(
                            text = {
                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    Text("Checker grid")
                                    Spacer(Modifier.weight(1f))
                                    if (state.showChecker) Icon(
                                        Icons.Default.Check, null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                }
                            },
                            onClick = { viewModel.toggleChecker() }
                        )
                        Divider()
                        DropdownMenuItem(
                            text = { Text("About") },
                            onClick = { showMenu = false; viewModel.showAbout(true) },
                            leadingIcon = { Icon(Icons.Default.Info, null) }
                        )
                    }
                }

                Text(
                    text = if (state.isLoaded) state.fileName else "Sprite-Tools",
                    style = MaterialTheme.typography.titleSmall,
                    color = SpriteColors.TextPrimary,
                    modifier = Modifier.weight(1f)
                )

                if (state.isLoaded) {
                    IconButton(onClick = { viewModel.toggleProperties() }) {
                        Icon(
                            Icons.Default.Info,
                            contentDescription = "Properties",
                            tint = if (state.showProperties) SpriteColors.Accent
                            else SpriteColors.TextDim,
                            modifier = Modifier.size(22.dp)
                        )
                    }
                }

                if (state.isLoaded) {
                    Text(
                        "${(state.zoom * 100).toInt()}%",
                        color = SpriteColors.TextDim,
                        style = MaterialTheme.typography.bodySmall,
                        modifier = Modifier.padding(end = 8.dp)
                    )
                }
            }

            AnimatedVisibility(
                visible = state.showToolbar && state.isLoaded,
                enter = expandVertically(),
                exit = shrinkVertically()
            ) {
                SpriteToolbar(
                    state = state,
                    onFirstFrame = { viewModel.firstFrame() },
                    onPrevFrame = { viewModel.prevFrame() },
                    onTogglePlay = { viewModel.togglePlayback() },
                    onNextFrame = { viewModel.nextFrame() },
                    onLastFrame = { viewModel.lastFrame() },
                    onSpeedChanged = { viewModel.setPlaybackSpeed(it) },
                    onZoomIn = { viewModel.zoomIn() },
                    onZoomOut = { viewModel.zoomOut() },
                    onResetZoom = { viewModel.resetZoom() },
                    onZoomChanged = { viewModel.setZoom(it) },
                    onFrameChanged = { viewModel.setFrame(it) },
                    onOpenFile = onOpenFile
                )
            }

            SpriteViewport(
                state = state,
                modifier = Modifier.weight(1f),
                onOffsetChanged = { x, y -> viewModel.setOffset(x, y) },
                onZoomChanged = { viewModel.setZoom(it) },
                onOpenFile = onOpenFile
            )

            StatusBar(state = state)
        }

        if (state.showProperties && state.isLoaded) {
            ModalBottomSheet(
                onDismissRequest = { viewModel.toggleProperties() },
                sheetState = sheetState,
                containerColor = SpriteColors.BgMid,
                shape = RoundedCornerShape(topStart = 12.dp, topEnd = 12.dp),
                dragHandle = {
                    Column(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Spacer(Modifier.height(8.dp))
                        Box(
                            modifier = Modifier
                                .width(32.dp)
                                .height(4.dp)
                                .background(
                                    SpriteColors.TextDim,
                                    RoundedCornerShape(2.dp)
                                )
                        )
                        Spacer(Modifier.height(4.dp))
                        Text(
                            "Properties",
                            color = SpriteColors.TextSection,
                            style = MaterialTheme.typography.titleSmall
                        )
                        Spacer(Modifier.height(8.dp))
                    }
                }
            ) {
                PropertiesPanel(
                    state = state,
                    modifier = Modifier
                        .fillMaxWidth()
                        .fillMaxHeight(0.85f)
                )
            }
        }
    }

    if (state.showAbout) {
        AboutDialog(onDismiss = { viewModel.showAbout(false) })
    }
}