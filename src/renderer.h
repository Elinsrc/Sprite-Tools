#pragma once

#include "sprite_loader.h"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>

class SpriteRenderer
{
public:
    SpriteRenderer() = default;
    ~SpriteRenderer() = default;

    void UploadFrame(SpriteFrame& frame);
    void DeleteTextures(SpriteData& data);
    void UploadAllFrames(SpriteData& data);
};
