#pragma once

#include "sprite_loader.h"

#ifdef _WIN32
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