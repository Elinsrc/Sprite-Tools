package com.spritetools.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.spritetools.R
import com.spritetools.core.SpriteInfo
import com.spritetools.ui.theme.SpriteColors
import com.spritetools.ui.theme.SpriteToolsTheme
import com.spritetools.viewmodel.SpriteUiState
import kotlinx.coroutines.delay

@Composable
fun StatusBar(
    state: SpriteUiState,
    modifier: Modifier = Modifier
) {
    var currentTime by remember { mutableLongStateOf(System.currentTimeMillis()) }

    LaunchedEffect(state.statusTimestamp, state.statusMessage) {
        if (state.statusMessage.isNotEmpty()) {
            while (true) {
                currentTime = System.currentTimeMillis()
                val age = currentTime - state.statusTimestamp
                if (age > 4000) break
                delay(32)
            }
        }
    }

    Row(
        modifier = modifier
            .fillMaxWidth()
            .height(24.dp)
            .background(SpriteColors.StatusBg)
            .padding(horizontal = 8.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        val age = (currentTime - state.statusTimestamp) / 1000.0
        if (state.statusMessage.isNotEmpty() && age < 4.0) {
            val alpha = if (age < 3.0) 1.0f else (4.0 - age).toFloat()
            Text(
                state.statusMessage,
                fontSize = 11.sp,
                color = SpriteColors.TextPrimary.copy(alpha = alpha.coerceIn(0f, 1f)),
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
                val context = LocalContext.current
                Text(
                    stringResource(R.string.info_format, info.version, info.getTexFormatName(context), state.totalFrames),
                    fontSize = 10.sp,
                    color = SpriteColors.TextDim.copy(alpha = 0.7f),
                    maxLines = 1
                )
            }
        }
    }
}

@Preview
@Composable
fun StatusBarPreview() {
    SpriteToolsTheme {
        StatusBar(
            state = SpriteUiState(
                isLoaded = true,
                statusMessage = "Sprite loaded successfully",
                statusTimestamp = System.currentTimeMillis(),
                totalFrames = 12,
                spriteInfo = SpriteInfo(
                    version = 2,
                    type = 0,
                    texFormat = 0,
                    facetype = 0,
                    boundsW = 64,
                    boundsH = 64,
                    numFrames = 12,
                    numGroups = 1,
                    paletteColors = 256
                )
            )
        )
    }
}
