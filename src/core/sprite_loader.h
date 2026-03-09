#pragma once

#include "sprite.h"
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <cstring>

struct SpriteFrame
{
    int32_t origin[2];
    int32_t width;
    int32_t height;
    std::vector<uint8_t> pixels;
    std::vector<uint8_t> rgba;
    float interval;
    unsigned int gl_texture;
};

struct SpriteFrameGroup
{
    frametype_t type;
    std::vector<SpriteFrame> frames;
    std::vector<float> intervals;
};

struct SpriteData
{
    int32_t version;
    uint32_t type;      
    uint32_t texFormat;      
    float boundingradius;
    int32_t bounds[2];
    int32_t numframes;
    uint32_t facetype;
    uint32_t synctype;
    int16_t palette_colors;
    uint8_t palette[256 * 3];

    std::vector<SpriteFrameGroup> groups;

    std::string filepath;
    bool loaded;

    SpriteData() : version(0), type(0), texFormat(0), boundingradius(0), numframes(0), facetype(0), synctype(0), palette_colors(0), loaded(false)
    {
        bounds[0] = bounds[1] = 0;
        memset(palette, 0, sizeof(palette));
    }
};

class SpriteLoader
{
public:
    SpriteLoader();
    ~SpriteLoader();

    bool Load(const std::string& filepath);
    void Unload();

    const SpriteData& GetData() const { return m_data; }
    SpriteData& GetData() { return m_data; }

    SpriteFrame* GetFrame(int index);
    int GetTotalFrameCount() const;

    static const char* GetTypeString(uint32_t type);
    static const char* GetTexFormatString(uint32_t fmt);
    static const char* GetFaceTypeString(uint32_t ft);

    bool LoadFromMemory(const uint8_t* data, size_t size, const std::string& name);

private:
    bool LoadQ1Sprite(const uint8_t* data, size_t size);
    bool LoadHLSprite(const uint8_t* data, size_t size);

    bool ReadFrame(const uint8_t*& ptr, const uint8_t* end, SpriteFrame& frame, bool is32bit);
    void ConvertFrameToRGBA(SpriteFrame& frame, bool is32bit);

    SpriteData m_data;
};
