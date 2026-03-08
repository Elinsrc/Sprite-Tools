#include "sprite_loader.h"
#include <cstdio>
#include <fstream>
#include <algorithm>

template<typename T>
static bool ReadVal(const uint8_t*& ptr, const uint8_t* end, T& val)
{
    if (ptr + sizeof(T) > end) return false;
    memcpy(&val, ptr, sizeof(T));
    ptr += sizeof(T);
    return true;
}

static bool ReadBytes(const uint8_t*& ptr, const uint8_t* end, void* dst, size_t count)
{
    if (ptr + count > end) return false;
    memcpy(dst, ptr, count);
    ptr += count;
    return true;
}

SpriteLoader::SpriteLoader() 
{
}

SpriteLoader::~SpriteLoader() 
{
    Unload(); 
}

void SpriteLoader::Unload()
{
    m_data = SpriteData();
}

bool SpriteLoader::Load(const std::string& filepath)
{
    Unload();

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        fprintf(stderr, "Cannot open file: %s\n", filepath.c_str());
        return false;
    }

    size_t filesize = (size_t)file.tellg();
    file.seekg(0, std::ios::beg);

    if (filesize < sizeof(dsprite_t))
    {
        fprintf(stderr, "File too small: %s\n", filepath.c_str());
        return false;
    }

    std::vector<uint8_t> buffer(filesize);
    file.read(reinterpret_cast<char*>(buffer.data()), filesize);
    file.close();

    dsprite_t header;
    memcpy(&header, buffer.data(), sizeof(header));

    if (header.ident != IDSPRITEHEADER)
    {
        fprintf(stderr, "Invalid sprite header in: %s (got 0x%08X, expected 0x%08X)\n", filepath.c_str(), header.ident, IDSPRITEHEADER);
        return false;
    }

    m_data.filepath = filepath;
    m_data.version = header.version;

    bool result = false;
    switch (header.version)
    {
    case SPRITE_VERSION_Q1:
        result = LoadQ1Sprite(buffer.data(), filesize);
        break;
    case SPRITE_VERSION_HL:
        result = LoadHLSprite(buffer.data(), filesize);
        break;
    default:
        fprintf(stderr, "Unknown sprite version: %d\n", header.version);
        return false;
    }

    m_data.loaded = result;
    return result;
}

bool SpriteLoader::LoadQ1Sprite(const uint8_t* data, size_t size)
{
    const uint8_t* ptr = data;
    const uint8_t* end = data + size;

    dsprite_q1_t header;
    if (!ReadBytes(ptr, end, &header, sizeof(header)))
        return false;

    m_data.type = (uint32_t)header.type;
    m_data.texFormat = SPR_NORMAL;
    m_data.boundingradius = header.boundingradius;
    m_data.bounds[0] = header.bounds[0];
    m_data.bounds[1] = header.bounds[1];
    m_data.numframes = header.numframes;
    m_data.synctype = header.synctype;
    m_data.facetype = SPR_CULL_FRONT;

    m_data.palette_colors = 256;
    for (int i = 0; i < 256; i++)
    {
        m_data.palette[i * 3 + 0] = (uint8_t)i;
        m_data.palette[i * 3 + 1] = (uint8_t)i;
        m_data.palette[i * 3 + 2] = (uint8_t)i;
    }

    for (int i = 0; i < header.numframes; i++)
    {
        dframetype_t ft;
        if (!ReadVal(ptr, end, ft))
            return false;

        SpriteFrameGroup group;
        group.type = (frametype_t)ft.type;

        if (ft.type == FRAME_SINGLE)
        {
            SpriteFrame frame;
            if (!ReadFrame(ptr, end, frame, false))
                return false;
            ConvertFrameToRGBA(frame, false);
            frame.interval = 0.1f;
            group.frames.push_back(std::move(frame));
        }
        else
        {
            dspritegroup_t sg;
            if (!ReadVal(ptr, end, sg))
                return false;

            for (int j = 0; j < sg.numframes; j++)
            {
                dspriteinterval_t interval;
                if (!ReadVal(ptr, end, interval))
                    return false;
                group.intervals.push_back(interval.interval);
            }

            for (int j = 0; j < sg.numframes; j++)
            {
                SpriteFrame frame;
                if (!ReadFrame(ptr, end, frame, false))
                    return false;
                ConvertFrameToRGBA(frame, false);
                frame.interval = (j < (int)group.intervals.size()) ? group.intervals[j] : 0.1f;
                group.frames.push_back(std::move(frame));
            }
        }

        m_data.groups.push_back(std::move(group));
    }

    return true;
}

bool SpriteLoader::LoadHLSprite(const uint8_t* data, size_t size)
{
    const uint8_t* ptr = data;
    const uint8_t* end = data + size;

    dsprite_hl_t header;
    if (!ReadBytes(ptr, end, &header, sizeof(header)))
        return false;

    m_data.type = header.type;
    m_data.texFormat = header.texFormat;
    m_data.boundingradius = (float)header.boundingradius;
    m_data.bounds[0] = header.bounds[0];
    m_data.bounds[1] = header.bounds[1];
    m_data.numframes = header.numframes;
    m_data.facetype = header.facetype;
    m_data.synctype = header.synctype;

    int16_t palsize;
    if (!ReadVal(ptr, end, palsize))
        return false;

    m_data.palette_colors = palsize;
    if (palsize > 0 && palsize <= 256)
    {
        if (!ReadBytes(ptr, end, m_data.palette, palsize * 3))
            return false;
    }

    for (int i = 0; i < header.numframes; i++)
    {
        dframetype_t ft;
        if (!ReadVal(ptr, end, ft))
            return false;

        SpriteFrameGroup group;
        group.type = (frametype_t)ft.type;

        if (ft.type == FRAME_SINGLE)
        {
            SpriteFrame frame;
            if (!ReadFrame(ptr, end, frame, false))
                return false;
            ConvertFrameToRGBA(frame, false);
            frame.interval = 0.1f;
            group.frames.push_back(std::move(frame));
        }
        else
        {
            dspritegroup_t sg;
            if (!ReadVal(ptr, end, sg))
                return false;

            for (int j = 0; j < sg.numframes; j++)
            {
                dspriteinterval_t interval;
                if (!ReadVal(ptr, end, interval))
                    return false;
                group.intervals.push_back(interval.interval);
            }

            for (int j = 0; j < sg.numframes; j++)
            {
                SpriteFrame frame;
                if (!ReadFrame(ptr, end, frame, false))
                    return false;
                ConvertFrameToRGBA(frame, false);
                frame.interval = (j < (int)group.intervals.size()) ? group.intervals[j] : 0.1f;
                group.frames.push_back(std::move(frame));
            }
        }

        m_data.groups.push_back(std::move(group));
    }

    return true;
}

bool SpriteLoader::ReadFrame(const uint8_t*& ptr, const uint8_t* end, SpriteFrame& frame, bool is32bit)
{
    dspriteframe_t fhdr;
    if (!ReadVal(ptr, end, fhdr))
        return false;

    frame.origin[0] = fhdr.origin[0];
    frame.origin[1] = fhdr.origin[1];
    frame.width = fhdr.width;
    frame.height = fhdr.height;
    frame.gl_texture = 0;

    if (frame.width <= 0 || frame.height <= 0 || frame.width > 4096 || frame.height > 4096)
    {
        fprintf(stderr, "Invalid frame size: %dx%d\n", frame.width, frame.height);
        return false;
    }

    size_t pixel_count = (size_t)frame.width * frame.height;
    size_t data_size = is32bit ? pixel_count * 4 : pixel_count;

    frame.pixels.resize(data_size);
    if (!ReadBytes(ptr, end, frame.pixels.data(), data_size))
        return false;

    return true;
}

void SpriteLoader::ConvertFrameToRGBA(SpriteFrame& frame, bool is32bit)
{
    size_t pixel_count = (size_t)frame.width * frame.height;
    frame.rgba.resize(pixel_count * 4);

    if (is32bit)
    {
        memcpy(frame.rgba.data(), frame.pixels.data(), pixel_count * 4);
    }
    else
    {
        const uint8_t* pal = m_data.palette;
        uint32_t texFormat = m_data.texFormat;

        for (size_t i = 0; i < pixel_count; i++)
        {
            uint8_t index = frame.pixels[i];
            uint8_t r = pal[index * 3 + 0];
            uint8_t g = pal[index * 3 + 1];
            uint8_t b = pal[index * 3 + 2];
            uint8_t a = 255;

            switch (texFormat)
            {
            case SPR_NORMAL:
                a = 255;
                break;
            case SPR_ADDITIVE:
                a = 255;
                break;
            case SPR_INDEXALPHA:
                a = index;
                r = pal[255 * 3 + 0];
                g = pal[255 * 3 + 1];
                b = pal[255 * 3 + 2];
                break;
            case SPR_ALPHTEST:
                if (index == 255)
                    a = 0;
                break;
            }

            frame.rgba[i * 4 + 0] = r;
            frame.rgba[i * 4 + 1] = g;
            frame.rgba[i * 4 + 2] = b;
            frame.rgba[i * 4 + 3] = a;
        }
    }
}

SpriteFrame* SpriteLoader::GetFrame(int index)
{
    int current = 0;
    for (auto& group : m_data.groups)
    {
        for (auto& frame : group.frames)
        {
            if (current == index)
                return &frame;
            current++;
        }
    }
    return nullptr;
}

int SpriteLoader::GetTotalFrameCount() const
{
    int count = 0;
    for (const auto& group : m_data.groups)
    {
        count += (int)group.frames.size();
    }
    return count;
}

const char* SpriteLoader::GetTypeString(uint32_t type)
{
    switch (type)
    {
    case SPR_FWD_PARALLEL_UPRIGHT:      
        return "Parallel Upright";
    case SPR_FACING_UPRIGHT:            
        return "Facing Upright";
    case SPR_FWD_PARALLEL:              
        return "Parallel";
    case SPR_ORIENTED:                  
        return "Oriented";
    case SPR_FWD_PARALLEL_ORIENTED:     
        return "Parallel Oriented";
    default:                            
        return "Unknown";
    }
}

const char* SpriteLoader::GetTexFormatString(uint32_t fmt)
{
    switch (fmt)
    {
    case SPR_NORMAL:       
        return "Normal";
    case SPR_ADDITIVE:      
        return "Additive";
    case SPR_INDEXALPHA:    
        return "Index Alpha";
    case SPR_ALPHTEST:      
        return "Alpha Test";
    default:                
        return "Unknown";
    }
}

const char* SpriteLoader::GetFaceTypeString(uint32_t ft)
{
    switch (ft)
    {
    case SPR_CULL_FRONT:    
        return "Cull Front";
    case SPR_CULL_NONE:    
        return "No Cull";
    default:                
        return "Unknown";
    }
}