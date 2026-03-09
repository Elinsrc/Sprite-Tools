package com.spritetools.ui.components

import android.os.Environment
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import com.spritetools.core.ImageExportFormat
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState
import java.io.File

@Composable
fun ExportDialog(
    state: SpriteUiState,
    onDismiss: () -> Unit,
    onExportAll: (File, String, ImageExportFormat) -> Unit,
    onExportCurrent: (File, ImageExportFormat) -> Unit
) {
    var selectedFormat by remember { mutableStateOf(0) }
    var exportAll by remember { mutableStateOf(true) }

    val formats = ImageExportFormat.entries.toList()

    Dialog(onDismissRequest = onDismiss) {
        Surface(
            shape = RoundedCornerShape(4.dp),
            color = SpriteColors.BgMid,
            border = androidx.compose.foundation.BorderStroke(1.dp, SpriteColors.Border),
            shadowElevation = 8.dp
        ) {
            Column(
                modifier = Modifier
                    .width(360.dp)
                    .padding(16.dp)
            ) {
                Text("Export Frames", fontSize = 14.sp, color = SpriteColors.TextPrimary)
                Spacer(Modifier.height(8.dp))

                SectionHeader("Format")

                formats.forEachIndexed { index, fmt ->
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(28.dp)
                            .padding(start = 8.dp)
                    ) {
                        RadioButton(
                            selected = selectedFormat == index,
                            onClick = { selectedFormat = index },
                            colors = RadioButtonDefaults.colors(
                                selectedColor = SpriteColors.Accent,
                                unselectedColor = SpriteColors.TextDim
                            ),
                            modifier = Modifier.scale(0.8f)
                        )
                        Text(
                            fmt.label,
                            fontSize = 13.sp,
                            color = SpriteColors.TextPrimary
                        )
                    }
                }

                SectionHeader("Selection")

                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.height(28.dp).padding(start = 8.dp)
                ) {
                    RadioButton(
                        selected = exportAll,
                        onClick = { exportAll = true },
                        colors = RadioButtonDefaults.colors(
                            selectedColor = SpriteColors.Accent,
                            unselectedColor = SpriteColors.TextDim
                        ),
                        modifier = Modifier.scale(0.8f)
                    )
                    Text("All frames (${state.totalFrames})", fontSize = 13.sp, color = SpriteColors.TextPrimary)
                }

                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.height(28.dp).padding(start = 8.dp)
                ) {
                    RadioButton(
                        selected = !exportAll,
                        onClick = { exportAll = false },
                        colors = RadioButtonDefaults.colors(
                            selectedColor = SpriteColors.Accent,
                            unselectedColor = SpriteColors.TextDim
                        ),
                        modifier = Modifier.scale(0.8f)
                    )
                    Text("Current frame (${state.currentFrame + 1})", fontSize = 13.sp, color = SpriteColors.TextPrimary)
                }

                Spacer(Modifier.height(16.dp))
                Divider(color = SpriteColors.Border)
                Spacer(Modifier.height(12.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.Center
                ) {
                    Button(
                        onClick = {
                            val fmt = formats[selectedFormat]
                            val exportDir = File(
                                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS),
                                "SpriteTools"
                            )
                            val baseName = state.fileName.substringBeforeLast(".").ifEmpty { "sprite" }

                            if (exportAll) {
                                onExportAll(exportDir, baseName, fmt)
                            } else {
                                val name = "${baseName}_${state.currentFrame}${fmt.ext}"
                                val file = File(exportDir, name)
                                onExportCurrent(file, fmt)
                            }
                        },
                        colors = ButtonDefaults.buttonColors(
                            containerColor = SpriteColors.BgLighter,
                            contentColor = SpriteColors.TextPrimary
                        ),
                        shape = RoundedCornerShape(3.dp),
                        modifier = Modifier.width(100.dp).height(32.dp),
                        contentPadding = PaddingValues(0.dp)
                    ) {
                        Text("Export", fontSize = 13.sp)
                    }

                    Spacer(Modifier.width(8.dp))

                    Button(
                        onClick = onDismiss,
                        colors = ButtonDefaults.buttonColors(
                            containerColor = SpriteColors.BgLighter,
                            contentColor = SpriteColors.TextPrimary
                        ),
                        shape = RoundedCornerShape(3.dp),
                        modifier = Modifier.width(100.dp).height(32.dp),
                        contentPadding = PaddingValues(0.dp)
                    ) {
                        Text("Cancel", fontSize = 13.sp)
                    }
                }
            }
        }
    }
}

@Composable
private fun SectionHeader(title: String) {
    Spacer(Modifier.height(8.dp))
    Row(
        modifier = Modifier.fillMaxWidth(),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Divider(modifier = Modifier.width(4.dp), color = SpriteColors.Border)
        Spacer(Modifier.width(6.dp))
        Text(title, fontSize = 13.sp, color = SpriteColors.TextSection)
        Spacer(Modifier.width(6.dp))
        Divider(modifier = Modifier.weight(1f), color = SpriteColors.Border)
    }
    Spacer(Modifier.height(4.dp))
}

fun Modifier.scale(scale: Float): Modifier = this.then(
    Modifier.size(48.dp * scale)
)