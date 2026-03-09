#include "sprite_converter_capi.h"
#include "sprite_converter.h"

#include <cstring>
#include <cstdlib>
#include <string>

static std::string g_conv_error;

extern "C" {

const char* sprite_converter_last_error(void)
{
    return g_conv_error.c_str();
}

void sprite_free_buffer(uint8_t* buffer)
{
    free(buffer);
}

int sprite_export_frames(const char* spr_path, const char* output_dir, const char* prefix, int format, int frame_index)
{
    SprToImageParams p;
    p.output_dir = output_dir ? output_dir : "";
    p.output_prefix = prefix ? prefix : "";
    p.format = static_cast<ImageFormat>(format);
    p.frame_index = frame_index;

    auto r = SpriteConverter::SprToImage(spr_path, p);
    if (!r.success) 
    { 
        g_conv_error = r.error; 
        return -1; 
    }
    g_conv_error.clear();
    return 0;
}

int sprite_export_frame_to_buffer(const uint8_t* spr_data, size_t spr_size, int frame_index, int format, uint8_t** out_data, size_t* out_size)
{
    if (!out_data || !out_size)
    {
        g_conv_error = "Null output pointers";
        return -1;
    }
    *out_data = nullptr;
    *out_size = 0;

    auto r = SpriteConverter::SprFrameToImageMemory(spr_data, spr_size, frame_index, static_cast<ImageFormat>(format));

    if (!r.success || r.data.empty())
    {
        g_conv_error = r.error;
        return -1;
    }

    *out_data = static_cast<uint8_t*>(malloc(r.data.size()));
    if (!*out_data)
    {
        g_conv_error = "malloc failed";
        return -1;
    }
    memcpy(*out_data, r.data.data(), r.data.size());
    *out_size = r.data.size();

    g_conv_error.clear();
    return 0;
}

int sprite_create_from_files(const char** image_paths, int image_count, const char* output_path, int version, int type, int tex_format, float interval)
{
    std::vector<std::string> paths;
    for (int i = 0; i < image_count; i++)
        paths.emplace_back(image_paths[i]);

    ImageToSprParams p;
    p.version = version;
    p.type = (uint32_t)type;
    p.tex_format = (uint32_t)tex_format;
    p.interval = interval;

    auto r = SpriteConverter::ImageToSpr(paths, output_path, p);
    if (!r.success) 
    { 
        g_conv_error = r.error; 
        return -1; 
    }
    g_conv_error.clear();
    return 0;
}

int sprite_create_from_buffers(const uint8_t** image_data_ptrs, const size_t* image_sizes, int image_count, int version, int type, int tex_format, float interval, uint8_t** out_spr_data, size_t* out_spr_size)
{
    if (!out_spr_data || !out_spr_size)
    {
        g_conv_error = "Null output pointers";
        return -1;
    }
    *out_spr_data = nullptr;
    *out_spr_size = 0;

    std::vector<const uint8_t*> ptrs(image_data_ptrs, image_data_ptrs + image_count);
    std::vector<size_t> sizes(image_sizes, image_sizes + image_count);

    ImageToSprParams p;
    p.version = version;
    p.type = (uint32_t)type;
    p.tex_format = (uint32_t)tex_format;
    p.interval = interval;

    auto r = SpriteConverter::ImageToSprMemory(ptrs, sizes, p);
    if (!r.success || r.data.empty())
    {
        g_conv_error = r.error;
        return -1;
    }

    *out_spr_data = static_cast<uint8_t*>(malloc(r.data.size()));
    if (!*out_spr_data)
    {
        g_conv_error = "malloc failed";
        return -1;
    }
    memcpy(*out_spr_data, r.data.data(), r.data.size());
    *out_spr_size = r.data.size();

    g_conv_error.clear();
    return 0;
}
}