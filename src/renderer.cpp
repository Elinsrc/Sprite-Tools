#include "renderer.h"
#include <cstdio>

void SpriteRenderer::UploadFrame(SpriteFrame& frame)
{
    if (frame.gl_texture != 0)
        return;

    if (frame.rgba.empty() || frame.width <= 0 || frame.height <= 0)
        return;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width, frame.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.rgba.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    frame.gl_texture = tex;
}

void SpriteRenderer::DeleteTextures(SpriteData& data)
{
    for (auto& group : data.groups)
    {
        for (auto& frame : group.frames)
        {
            if (frame.gl_texture != 0)
            {
                GLuint tex = frame.gl_texture;
                glDeleteTextures(1, &tex);
                frame.gl_texture = 0;
            }
        }
    }
}

void SpriteRenderer::UploadAllFrames(SpriteData& data)
{
    for (auto& group : data.groups)
    {
        for (auto& frame : group.frames)
        {
            UploadFrame(frame);
        }
    }
}