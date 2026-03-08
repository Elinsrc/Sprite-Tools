package com.spritetools.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.viewmodel.SpriteUiState

@Composable
fun StatusBar(
    state: SpriteUiState,
    modifier: Modifier = Modifier
) {
    Row(
        modifier = modifier
            .fillMaxWidth()
            .height(24.dp)
            .background(SpriteColors.StatusBg)
            .padding(horizontal = 8.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        val age = (System.currentTimeMillis() - state.statusTimestamp) / 1000.0
        if (state.statusMessage.isNotEmpty() && age < 4.0) {
            val alpha = if (age < 3.0) 1.0f else (4.0 - age).toFloat()
            Text(
                state.statusMessage,
                fontSize = 11.sp,
                color = SpriteColors.TextPrimary.copy(alpha = alpha),
                maxLines = 1,
                overflow = TextOverflow.Ellipsis,
                modifier = Modifier.weight(1f)
            )
        } else {
            Spacer(Modifier.weight(1f))
        }

        if (state.isLoaded) {
            val info = state.spriteInfo
            if (info != null) {
                Text(
                    "v${info.version} ${info.texFormatName} ${state.totalFrames}fr",
                    fontSize = 10.sp,
                    color = SpriteColors.TextDim.copy(alpha = 0.7f),
                    maxLines = 1
                )
            }
        }
    }
}