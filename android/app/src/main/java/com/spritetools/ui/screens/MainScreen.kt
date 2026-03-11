package com.spritetools.ui.screens

import android.net.Uri
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
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.spritetools.R
import com.spritetools.core.*
import com.spritetools.ui.components.*
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme
import com.spritetools.viewmodel.SpriteUiState
import com.spritetools.viewmodel.SpriteViewModel
import java.io.File

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    viewModel: SpriteViewModel,
    onOpenFile: () -> Unit
) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    MainScreenContent(
        state = state,
        onOpenFile = onOpenFile,
        onCloseFile = { viewModel.closeFile() },
        onShowExportDialog = { viewModel.showExportDialog(it) },
        onShowImportDialog = { viewModel.showImportDialog(it) },
        onToggleToolbar = { viewModel.toggleToolbar() },
        onToggleChecker = { viewModel.toggleChecker() },
        onShowAbout = { viewModel.showAbout(it) },
        onToggleProperties = { viewModel.toggleProperties() },
        onFirstFrame = { viewModel.firstFrame() },
        onPrevFrame = { viewModel.prevFrame() },
        onTogglePlay = { viewModel.togglePlayback() },
        onNextFrame = { viewModel.nextFrame() },
        onLastFrame = { viewModel.lastFrame() },
        onSpeedChanged = { viewModel.setPlaybackSpeed(it) },
        onZoomIn = { viewModel.zoomIn() },
        onZoomOut = { viewModel.zoomOut() },
        onResetZoom = { viewModel.resetZoom() },
        onFrameChanged = { viewModel.setFrame(it) },
        onOffsetChanged = { x, y -> viewModel.setOffset(x, y) },
        onZoomChanged = { viewModel.setZoom(it) },
        onViewportSizeChanged = { w, h -> viewModel.onViewportSizeChanged(w, h) },
        onExportAll = { dir, name, fmt -> viewModel.exportAllFrames(dir, name, fmt) },
        onExportCurrent = { file, fmt -> viewModel.exportCurrentFrame(file, fmt) },
        onCreateSpr = { uris, file, ver, type, fmt, interval ->
            viewModel.createSprFromUris(uris, file, ver, type, fmt, interval)
        },
        onCancelConvert = { viewModel.cancelConvert() },
        onDismissProgress = { viewModel.dismissProgress() }
    )
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreenContent(
    state: SpriteUiState,
    onOpenFile: () -> Unit,
    onCloseFile: () -> Unit,
    onShowExportDialog: (Boolean) -> Unit,
    onShowImportDialog: (Boolean) -> Unit,
    onToggleToolbar: () -> Unit,
    onToggleChecker: () -> Unit,
    onShowAbout: (Boolean) -> Unit,
    onToggleProperties: () -> Unit,
    onFirstFrame: () -> Unit,
    onPrevFrame: () -> Unit,
    onTogglePlay: () -> Unit,
    onNextFrame: () -> Unit,
    onLastFrame: () -> Unit,
    onSpeedChanged: (Float) -> Unit,
    onZoomIn: () -> Unit,
    onZoomOut: () -> Unit,
    onResetZoom: () -> Unit,
    onFrameChanged: (Int) -> Unit,
    onOffsetChanged: (Float, Float) -> Unit,
    onZoomChanged: (Float) -> Unit,
    onViewportSizeChanged: (Int, Int) -> Unit,
    onExportAll: (File, String, ImageExportFormat) -> Unit,
    onExportCurrent: (File, ImageExportFormat) -> Unit,
    onCreateSpr: (List<Uri>, File, Int, SprType, SprTexFormat, Float) -> Unit,
    onCancelConvert: () -> Unit,
    onDismissProgress: () -> Unit
) {
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
                            contentDescription = stringResource(R.string.app_name),
                            tint = SpriteColors.TextPrimary,
                            modifier = Modifier.size(22.dp)
                        )
                    }
                    DropdownMenu(
                        expanded = showMenu,
                        onDismissRequest = { showMenu = false }
                    ) {
                        DropdownMenuItem(
                            text = { Text(stringResource(R.string.menu_open)) },
                            onClick = { showMenu = false; onOpenFile() },
                            leadingIcon = { Icon(Icons.Default.FolderOpen, null) }
                        )
                        if (state.isLoaded) {
                            DropdownMenuItem(
                                text = { Text(stringResource(R.string.menu_close)) },
                                onClick = { showMenu = false; onCloseFile() },
                                leadingIcon = { Icon(Icons.Default.Close, null) }
                            )
                        }

                        Divider()

                        DropdownMenuItem(
                            text = { Text(stringResource(R.string.menu_export_frames)) },
                            onClick = {
                                showMenu = false
                                onShowExportDialog(true)
                            },
                            enabled = state.isLoaded,
                            leadingIcon = { Icon(Icons.Default.SaveAlt, null) }
                        )

                        DropdownMenuItem(
                            text = { Text(stringResource(R.string.menu_create_spr)) },
                            onClick = {
                                showMenu = false
                                onShowImportDialog(true)
                            },
                            leadingIcon = { Icon(Icons.Default.AddPhotoAlternate, null) }
                        )

                        Divider()

                        DropdownMenuItem(
                            text = {
                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    Text(stringResource(R.string.menu_toolbar))
                                    Spacer(Modifier.weight(1f))
                                    if (state.showToolbar) Icon(
                                        Icons.Default.Check, null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                }
                            },
                            onClick = { onToggleToolbar() }
                        )
                        DropdownMenuItem(
                            text = {
                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    Text(stringResource(R.string.menu_checker_grid))
                                    Spacer(Modifier.weight(1f))
                                    if (state.showChecker) Icon(
                                        Icons.Default.Check, null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                }
                            },
                            onClick = { onToggleChecker() }
                        )
                        Divider()
                        DropdownMenuItem(
                            text = { Text(stringResource(R.string.menu_about)) },
                            onClick = { showMenu = false; onShowAbout(true) },
                            leadingIcon = { Icon(Icons.Default.Info, null) }
                        )
                    }
                }

                Text(
                    text = if (state.isLoaded) state.fileName else stringResource(R.string.app_name),
                    style = MaterialTheme.typography.titleSmall,
                    color = SpriteColors.TextPrimary,
                    modifier = Modifier.weight(1f)
                )

                if (state.isLoaded) {
                    IconButton(onClick = { onToggleProperties() }) {
                        Icon(
                            Icons.Default.Info,
                            contentDescription = stringResource(R.string.properties_title),
                            tint = if (state.showProperties) SpriteColors.Accent
                            else SpriteColors.TextDim,
                            modifier = Modifier.size(22.dp)
                        )
                    }
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
                    onFirstFrame = onFirstFrame,
                    onPrevFrame = onPrevFrame,
                    onTogglePlay = onTogglePlay,
                    onNextFrame = onNextFrame,
                    onLastFrame = onLastFrame,
                    onSpeedChanged = onSpeedChanged,
                    onZoomIn = onZoomIn,
                    onZoomOut = onZoomOut,
                    onResetZoom = onResetZoom,
                    onFrameChanged = onFrameChanged
                )
            }

            SpriteViewport(
                state = state,
                modifier = Modifier.weight(1f),
                onOffsetChanged = onOffsetChanged,
                onZoomChanged = onZoomChanged,
                onViewportSizeChanged = onViewportSizeChanged,
            )

            StatusBar(state = state)
        }

        if (state.showProperties && state.isLoaded) {
            ModalBottomSheet(
                onDismissRequest = { onToggleProperties() },
                sheetState = sheetState,
                containerColor = SpriteColors.BgMid,
                shape = RoundedCornerShape(12.dp, 12.dp, 0.dp, 0.dp),
                dragHandle = {
                    Column(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Spacer(Modifier.height(8.dp))
                        Box(
                            Modifier
                                .width(32.dp)
                                .height(4.dp)
                                .background(SpriteColors.TextDim, RoundedCornerShape(2.dp))
                        )
                        Spacer(Modifier.height(4.dp))
                        Text(stringResource(R.string.properties_title),
                            color = SpriteColors.TextSection,
                            style = MaterialTheme.typography.titleSmall)
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
        AboutDialog(onDismiss = { onShowAbout(false) })
    }

    if (state.showExportDialog && state.isLoaded) {
        ExportDialog(
            state = state,
            onDismiss = { onShowExportDialog(false) },
            onExportAll = onExportAll,
            onExportCurrent = onExportCurrent
        )
    }

    if (state.showImportDialog) {
        ImportDialog(
            onDismiss = { onShowImportDialog(false) },
            onCreateSpr = onCreateSpr
        )
    }

    if (state.showProgress) {
        ProgressDialog(
            title = state.progressTitle,
            status = state.progressStatus,
            progress = state.progressValue,
            isDone = state.progressDone,
            isSuccess = state.progressSuccess,
            resultMessage = state.progressResult,
            onCancel = onCancelConvert,
            onDismiss = onDismissProgress
        )
    }
}

@Preview(showBackground = true)
@Composable
fun MainScreenPreview() {
    SpriteToolsTheme {
        MainScreenContent(
            state = SpriteUiState(
                isLoaded = true,
                fileName = "sample.spr",
                totalFrames = 1,
                zoom = 1.0f
            ),
            onOpenFile = {},
            onCloseFile = {},
            onShowExportDialog = {},
            onShowImportDialog = {},
            onToggleToolbar = {},
            onToggleChecker = {},
            onShowAbout = {},
            onToggleProperties = {},
            onFirstFrame = {},
            onPrevFrame = {},
            onTogglePlay = {},
            onNextFrame = {},
            onLastFrame = {},
            onSpeedChanged = {},
            onZoomIn = {},
            onZoomOut = {},
            onResetZoom = {},
            onFrameChanged = {},
            onOffsetChanged = { _, _ -> },
            onZoomChanged = {},
            onViewportSizeChanged = { _, _ -> },
            onExportAll = { _, _, _ -> },
            onExportCurrent = { _, _ -> },
            onCreateSpr = { _, _, _, _, _, _ -> },
            onCancelConvert = {},
            onDismissProgress = {}
        )
    }
}
