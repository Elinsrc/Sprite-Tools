package com.spritetools.ui.components

import android.content.Context
import android.net.Uri
import android.os.Environment
import android.provider.OpenableColumns
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import com.spritetools.core.SprTexFormat
import com.spritetools.core.SprType
import com.spritetools.ui.theme.SpriteColors
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import com.spritetools.R
import com.spritetools.ui.theme.SpriteToolsTheme
import java.io.File

private fun getFileName(context: Context, uri: Uri, unknown: String): String {
    if (uri.scheme == "content") {
        context.contentResolver.query(uri, null, null, null, null)?.use { cursor ->
            val nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
            if (nameIndex != -1 && cursor.moveToFirst()) {
                val name = cursor.getString(nameIndex)
                if (!name.isNullOrEmpty()) return name
            }
        }
    }

    uri.lastPathSegment?.let { segment ->
        val cleaned = segment.substringAfterLast('/')
        if (cleaned.isNotEmpty()) return cleaned
    }

    return unknown
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ImportDialog(
    onDismiss: () -> Unit,
    onCreateSpr: (List<Uri>, File, Int, SprType, SprTexFormat, Float) -> Unit
) {
    val context = LocalContext.current
    val unknownStr = stringResource(R.string.import_unknown_file)

    var selectedImages by remember { mutableStateOf<List<Uri>>(emptyList()) }
    var version by remember { mutableStateOf(2) }
    var selectedType by remember { mutableStateOf(2) }
    var selectedTexFmt by remember { mutableStateOf(0) }
    var interval by remember { mutableStateOf(0.1f) }
    var outputName by remember { mutableStateOf("output") }

    val fileNames = remember { mutableStateMapOf<Uri, String>() }

    val imagePicker = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenMultipleDocuments()
    ) { uris ->
        if (uris.isNotEmpty()) {
            selectedImages = selectedImages + uris
            uris.forEach { uri ->
                if (uri !in fileNames) {
                    fileNames[uri] = getFileName(context, uri, unknownStr)
                }
            }
        }
    }

    Dialog(onDismissRequest = onDismiss) {
        Surface(
            shape = RoundedCornerShape(4.dp),
            color = SpriteColors.BgMid,
            border = BorderStroke(1.dp, SpriteColors.Border),
            shadowElevation = 8.dp
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp)
                    .heightIn(max = 600.dp)
            ) {
                Text(stringResource(R.string.create_spr_title), fontSize = 14.sp, color = SpriteColors.TextPrimary)
                Spacer(Modifier.height(8.dp))

                SectionHeader(stringResource(R.string.create_section_input))

                Row(verticalAlignment = Alignment.CenterVertically) {
                    Button(stringResource(R.string.btn_add_images)) { imagePicker.launch(arrayOf("image/*")) }
                    Spacer(Modifier.width(8.dp))
                    if (selectedImages.isNotEmpty()) {
                        Button(stringResource(R.string.btn_clear)) {
                            selectedImages = emptyList()
                            fileNames.clear()
                        }
                    }
                }

                Spacer(Modifier.height(4.dp))
                Text(stringResource(R.string.create_num_images, selectedImages.size), fontSize = 12.sp, color = SpriteColors.TextDim)

                if (selectedImages.isNotEmpty()) {
                    Spacer(Modifier.height(4.dp))
                    Box(
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(100.dp)
                            .background(SpriteColors.BgDark)
                            .border(1.dp, SpriteColors.Border)
                    ) {
                        LazyColumn(
                            modifier = Modifier.padding(4.dp)
                        ) {
                            itemsIndexed(selectedImages) { index, uri ->
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    modifier = Modifier.fillMaxWidth()
                                ) {
                                    Text(
                                        fileNames[uri] ?: "image_$index",
                                        fontSize = 12.sp,
                                        color = SpriteColors.TextPrimary,
                                        modifier = Modifier.weight(1f),
                                        maxLines = 1
                                    )
                                    Spacer(Modifier.width(8.dp))
                                    Icon(
                                        Icons.Default.Close,
                                        contentDescription = null,
                                        tint = SpriteColors.TextDim,
                                        modifier = Modifier
                                            .size(16.dp)
                                            .clickable {
                                                val removed = selectedImages[index]
                                                selectedImages = selectedImages
                                                    .toMutableList()
                                                    .also { it.removeAt(index) }
                                                fileNames.remove(removed)
                                            }
                                    )
                                }
                            }
                        }
                    }
                }

                SectionHeader(stringResource(R.string.create_section_settings))

                TextField(
                    value = outputName,
                    onValueChange = { outputName = it },
                    label = stringResource(R.string.create_label_name)
                )

                Row(
                    modifier = Modifier.padding(vertical = 4.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(stringResource(R.string.create_label_version), fontSize = 12.sp, color = SpriteColors.TextDim, modifier = Modifier.width(70.dp))
                    RadioButton(selected = version == 1, onClick = { version = 1 }, label = stringResource(R.string.version_quake))
                    Spacer(Modifier.width(8.dp))
                    RadioButton(selected = version == 2, onClick = { version = 2 }, label = stringResource(R.string.version_hl))
                }

                Dropdown(
                    label = stringResource(R.string.create_label_type),
                    selected = SprType.entries[selectedType].getLabel(context),
                    items = SprType.entries.map { it.getLabel(context) },
                    onSelect = { selectedType = it }
                )

                if (version == 2) {
                    Dropdown(
                        label = stringResource(R.string.create_label_render),
                        selected = SprTexFormat.entries[selectedTexFmt].getLabel(context),
                        items = SprTexFormat.entries.map { it.getLabel(context) },
                        onSelect = { selectedTexFmt = it }
                    )
                }

                Spacer(Modifier.height(4.dp))
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Text(stringResource(R.string.create_label_interval), fontSize = 12.sp, color = SpriteColors.TextDim, modifier = Modifier.width(70.dp))
                    Text(stringResource(R.string.prop_interval_val_short, interval), fontSize = 12.sp, color = SpriteColors.TextPrimary)
                }
                Slider(
                    value = interval,
                    onValueChange = { interval = it },
                    valueRange = 0.01f..1.0f,
                    colors = SliderDefaults.colors(
                        thumbColor = SpriteColors.AccentDim,
                        activeTrackColor = SpriteColors.Accent,
                        inactiveTrackColor = SpriteColors.BgDark
                    ),
                    modifier = Modifier.height(20.dp)
                )

                Spacer(Modifier.height(16.dp))
                Divider(color = SpriteColors.Border)
                Spacer(Modifier.height(12.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.Center
                ) {
                    Button(
                        text = stringResource(R.string.btn_create),
                        enabled = selectedImages.isNotEmpty(),
                        width = 100.dp,
                        onClick = {
                            val dir = File(
                                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS),
                                "SpriteTools"
                            )
                            val name = outputName.ifEmpty { "output" }
                            val outFile = File(dir, "$name.spr")
                            onCreateSpr(
                                selectedImages, outFile, version,
                                SprType.entries[selectedType],
                                SprTexFormat.entries[selectedTexFmt],
                                interval
                            )
                        }
                    )
                    Spacer(Modifier.width(8.dp))
                    Button(text = stringResource(R.string.btn_cancel), width = 100.dp, onClick = onDismiss)
                }
            }
        }
    }
}

@Composable
private fun SectionHeader(title: String) {
    Spacer(Modifier.height(12.dp))
    Row(verticalAlignment = Alignment.CenterVertically) {
        Divider(modifier = Modifier.width(4.dp), color = SpriteColors.Border)
        Spacer(Modifier.width(6.dp))
        Text(title, fontSize = 13.sp, color = SpriteColors.TextSection)
        Spacer(Modifier.width(6.dp))
        Divider(modifier = Modifier.weight(1f), color = SpriteColors.Border)
    }
    Spacer(Modifier.height(6.dp))
}

@Composable
private fun Button(text: String, width: androidx.compose.ui.unit.Dp? = null, enabled: Boolean = true, onClick: () -> Unit) {
    Button(
        onClick = onClick,
        enabled = enabled,
        shape = RoundedCornerShape(3.dp),
        colors = ButtonDefaults.buttonColors(
            containerColor = SpriteColors.BgLighter,
            contentColor = SpriteColors.TextPrimary,
            disabledContainerColor = SpriteColors.BgDark,
            disabledContentColor = SpriteColors.TextDim
        ),
        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 0.dp),
        modifier = Modifier.height(26.dp).then(if (width != null) Modifier.width(width) else Modifier)
    ) {
        Text(text, fontSize = 13.sp)
    }
}

@Composable
private fun TextField(value: String, onValueChange: (String) -> Unit, label: String) {
    Row(
        modifier = Modifier.padding(vertical = 4.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(label, fontSize = 12.sp, color = SpriteColors.TextDim, modifier = Modifier.width(70.dp))
        BasicTextField(
            value = value,
            onValueChange = onValueChange,
            textStyle = TextStyle(color = SpriteColors.TextPrimary, fontSize = 13.sp),
            cursorBrush = androidx.compose.ui.graphics.SolidColor(SpriteColors.Accent),
            modifier = Modifier
                .background(SpriteColors.BgLight, RoundedCornerShape(3.dp))
                .border(1.dp, SpriteColors.BgDark, RoundedCornerShape(3.dp))
                .padding(horizontal = 6.dp, vertical = 4.dp)
                .fillMaxWidth()
        )
    }
}

@Composable
private fun RadioButton(selected: Boolean, onClick: () -> Unit, label: String) {
    Row(
        modifier = Modifier.clickable { onClick() },
        verticalAlignment = Alignment.CenterVertically
    ) {
        RadioButton(
            selected = selected,
            onClick = onClick,
            colors = RadioButtonDefaults.colors(selectedColor = SpriteColors.Accent, unselectedColor = SpriteColors.TextDim),
            modifier = Modifier.size(24.dp)
        )
        Text(label, fontSize = 13.sp, color = SpriteColors.TextPrimary)
    }
}

@Composable
private fun Dropdown(label: String, selected: String, items: List<String>, onSelect: (Int) -> Unit) {
    var expanded by remember { mutableStateOf(false) }
    Row(
        modifier = Modifier.padding(vertical = 4.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(label, fontSize = 12.sp, color = SpriteColors.TextDim, modifier = Modifier.width(70.dp))
        Box(modifier = Modifier.fillMaxWidth()) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .background(SpriteColors.BgLight, RoundedCornerShape(3.dp))
                    .clickable { expanded = true }
                    .padding(horizontal = 6.dp, vertical = 4.dp),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(selected, fontSize = 13.sp, color = SpriteColors.TextPrimary)
                Text("▼", fontSize = 10.sp, color = SpriteColors.TextDim)
            }
            DropdownMenu(
                expanded = expanded,
                onDismissRequest = { expanded = false },
                modifier = Modifier.background(SpriteColors.BgMid)
            ) {
                items.forEachIndexed { i, item ->
                    DropdownMenuItem(
                        text = { Text(item, fontSize = 13.sp, color = SpriteColors.TextPrimary) },
                        onClick = { onSelect(i); expanded = false },
                        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 0.dp),
                        modifier = Modifier.height(32.dp)
                    )
                }
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun ImportDialogPreview() {
    SpriteToolsTheme {
        ImportDialog(
            onDismiss = {},
            onCreateSpr = { _, _, _, _, _, _ -> }
        )
    }
}
