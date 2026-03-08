package com.spritetools.ui

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import com.spritetools.ui.screens.MainScreen
import com.spritetools.ui.theme.SpriteToolsTheme
import com.spritetools.viewmodel.SpriteViewModel

class MainActivity : ComponentActivity() {

    private val viewModel: SpriteViewModel by viewModels()

    private val openFileLauncher = registerForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        uri?.let { viewModel.loadFile(it) }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        enableEdgeToEdge()

        intent?.data?.let { uri ->
            viewModel.loadFile(uri)
        }

        setContent {
            SpriteToolsTheme {
                MainScreen(
                    viewModel = viewModel,
                    onOpenFile = {
                        openFileLauncher.launch(arrayOf("*/*"))
                    }
                )
            }
        }
    }
}