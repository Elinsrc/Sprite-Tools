#include "ui_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef PLATFORM_WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>

ImGuiImage UI_utils::LoadImageFromMemory(const unsigned char* buffer, int bufferSize)
{
    ImGuiImage result;

    int channels;
    unsigned char* data = stbi_load_from_memory(buffer, bufferSize, &result.width, &result.height, &channels, 4);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    result.texture = (ImTextureID)(intptr_t)texture;
    return result;
}