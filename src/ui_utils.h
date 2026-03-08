#pragma once

#include "imgui.h"

struct ImGuiImage
{
    ImTextureID texture = 0;
    int width = 0;
    int height = 0;
};

class UI_utils
{
public:
    UI_utils() = default;
    ~UI_utils() = default;

    static ImGuiImage LoadImageFromMemory(const unsigned char* buffer, int bufferSize);
};
