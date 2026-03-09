package com.spritetools.ui.screens

import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextDecoration
import androidx.compose.ui.text.withStyle
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.compose.foundation.text.ClickableText
import com.spritetools.ui.theme.SpriteColors

@Composable
fun AboutDialog(onDismiss: () -> Unit) {

    val context = LocalContext.current
    val url = "https://github.com/Elinsrc/Sprite-Tools"

    val annotatedLink = buildAnnotatedString {

        withStyle(
            SpanStyle(
                color = SpriteColors.TextPrimary
            )
        ) {
            append("Github: ")
        }

        pushStringAnnotation(
            tag = "URL",
            annotation = url
        )

        withStyle(
            SpanStyle(
                color = SpriteColors.Accent,
                textDecoration = TextDecoration.Underline,
                fontWeight = FontWeight.Medium
            )
        ) {
            append(url)
        }

        pop()
    }

    Dialog(onDismissRequest = onDismiss) {
        Surface(
            shape = RoundedCornerShape(6.dp),
            color = SpriteColors.BgMid,
            border = BorderStroke(1.dp, SpriteColors.Border),
            shadowElevation = 8.dp
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(18.dp)
            ) {

                Text(
                    "About",
                    fontSize = 14.sp,
                    color = SpriteColors.TextPrimary
                )

                Spacer(Modifier.height(10.dp))

                Text(
                    "Sprite-Tools",
                    fontSize = 17.sp,
                    fontWeight = FontWeight.Bold,
                    color = SpriteColors.Accent
                )

                Spacer(Modifier.height(6.dp))

                Text(
                    "Sprite viewer and creator for Quake / Half-Life sprite formats",
                    fontSize = 12.sp,
                    color = SpriteColors.TextPrimary,
                    lineHeight = 18.sp
                )

                Spacer(Modifier.height(16.dp))

                ClickableText(
                    text = annotatedLink,
                    style = LocalTextStyle.current.copy(fontSize = 12.sp),
                    onClick = { offset ->
                        annotatedLink.getStringAnnotations(
                            tag = "URL",
                            start = offset,
                            end = offset
                        ).firstOrNull()?.let {
                            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(it.item))
                            context.startActivity(intent)
                        }
                    }
                )

                Spacer(Modifier.height(16.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.Center
                ) {
                    Button(
                        onClick = onDismiss,
                        shape = RoundedCornerShape(3.dp),
                        colors = ButtonDefaults.buttonColors(
                            containerColor = SpriteColors.BgLighter,
                            contentColor = SpriteColors.TextPrimary
                        ),
                        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 0.dp),
                        modifier = Modifier
                            .height(26.dp)
                            .width(100.dp)
                    ) {
                        Text("OK", fontSize = 13.sp)
                    }
                }
            }
        }
    }
}