package com.spritetools.ui.theme

import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val DarkColorScheme = darkColorScheme(
    primary = SpriteColors.Accent,
    onPrimary = Color.White,
    secondary = SpriteColors.AccentDim,
    background = SpriteColors.BgDark,
    surface = SpriteColors.BgMid,
    surfaceVariant = SpriteColors.BgLight,
    onBackground = SpriteColors.TextPrimary,
    onSurface = SpriteColors.TextPrimary,
    onSurfaceVariant = SpriteColors.TextDim,
    outline = SpriteColors.Border
)

@Composable
fun SpriteToolsTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = DarkColorScheme,
        content = content
    )
}
