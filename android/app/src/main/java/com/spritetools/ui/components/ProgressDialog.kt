package com.spritetools.ui.components

import androidx.compose.animation.core.*
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.compose.ui.window.DialogProperties
import com.spritetools.R
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme

@Composable
fun ProgressDialog(
    title: String,
    status: String,
    progress: Float,
    isDone: Boolean,
    isSuccess: Boolean,
    resultMessage: String,
    onCancel: () -> Unit,
    onDismiss: () -> Unit
) {
    Dialog(
        onDismissRequest = { if (isDone) onDismiss() },
        properties = DialogProperties(dismissOnBackPress = isDone, dismissOnClickOutside = isDone)
    ) {
        Surface(
            shape = RoundedCornerShape(4.dp),
            color = SpriteColors.BgMid,
            border = BorderStroke(1.dp, SpriteColors.Border),
            shadowElevation = 8.dp,
            modifier = Modifier.width(360.dp)
        ) {
            Column(
                modifier = Modifier.padding(16.dp),
                horizontalAlignment = Alignment.Start
            ) {
                Text(title, fontSize = 14.sp, color = SpriteColors.TextSection)
                Spacer(Modifier.height(12.dp))

                if (!isDone) {
                    Text(status, fontSize = 13.sp, color = SpriteColors.TextPrimary)
                    Spacer(Modifier.height(8.dp))

                    Box(contentAlignment = Alignment.Center) {
                        LinearProgressIndicator(
                            progress = progress,
                            modifier = Modifier.fillMaxWidth().height(16.dp),
                            color = SpriteColors.AccentDim,
                            trackColor = SpriteColors.BgDark
                        )
                        Text("${(progress * 100).toInt()}%", fontSize = 11.sp, color = SpriteColors.TextPrimary)
                    }

                    Spacer(Modifier.height(8.dp))

                    val infiniteTransition = rememberInfiniteTransition(label = "")
                    val dots by infiniteTransition.animateFloat(
                        initialValue = 0f, targetValue = 4f,
                        animationSpec = infiniteRepeatable(tween(1000, easing = LinearEasing), RepeatMode.Restart),
                        label = ""
                    )
                    Text(stringResource(R.string.progress_working) + ".".repeat(dots.toInt()), fontSize = 12.sp, color = SpriteColors.TextDim)

                    Spacer(Modifier.height(12.dp))
                    Divider(color = SpriteColors.Border)
                    Spacer(Modifier.height(8.dp))

                    Box(Modifier.fillMaxWidth(), contentAlignment = Alignment.Center) {
                        Button(stringResource(R.string.btn_cancel), width = 100.dp, onClick = onCancel)
                    }
                } else {
                    Text(
                        if (isSuccess) stringResource(R.string.progress_success) else stringResource(R.string.progress_failed),
                        fontSize = 14.sp,
                        color = if (isSuccess) SpriteColors.Success else SpriteColors.Error
                    )
                    Spacer(Modifier.height(8.dp))

                    if (resultMessage.isNotEmpty()) {
                        Text(resultMessage, fontSize = 12.sp, color = SpriteColors.TextPrimary)
                        Spacer(Modifier.height(8.dp))
                    }

                    LinearProgressIndicator(
                        progress = 1f,
                        modifier = Modifier.fillMaxWidth().height(16.dp),
                        color = if (isSuccess) SpriteColors.Success else SpriteColors.Error,
                        trackColor = SpriteColors.BgDark
                    )

                    Spacer(Modifier.height(12.dp))
                    Divider(color = SpriteColors.Border)
                    Spacer(Modifier.height(8.dp))

                    Box(Modifier.fillMaxWidth(), contentAlignment = Alignment.Center) {
                        Button(stringResource(R.string.btn_ok), width = 100.dp, onClick = onDismiss)
                    }
                }
            }
        }
    }
}

@Composable
private fun Button(text: String, width: androidx.compose.ui.unit.Dp? = null, onClick: () -> Unit) {
    Button(
        onClick = onClick,
        shape = RoundedCornerShape(3.dp),
        colors = ButtonDefaults.buttonColors(
            containerColor = SpriteColors.BgLighter,
            contentColor = SpriteColors.TextPrimary
        ),
        contentPadding = PaddingValues(0.dp),
        modifier = Modifier.height(26.dp).then(if (width != null) Modifier.width(width) else Modifier)
    ) {
        Text(text, fontSize = 13.sp)
    }
}

@Preview(showBackground = true)
@Composable
fun ProgressDialogLoadingPreview() {
    SpriteToolsTheme {
        ProgressDialog(
            title = "Exporting Sprite",
            status = "Processing frames...",
            progress = 0.45f,
            isDone = false,
            isSuccess = false,
            resultMessage = "",
            onCancel = {},
            onDismiss = {}
        )
    }
}
