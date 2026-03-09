#pragma once

#include "sprite.h"
#include <cstdint>
#include <string>
#include <vector>

enum class ImageFormat
{
    PNG = 0,
    BMP = 1
};

struct SprToImageParams
{
    int frame_index = -1;
    ImageFormat format = ImageFormat::PNG;
    std::string output_dir;
    std::string output_prefix;
};

struct ImageToSprParams
{
    int32_t version = SPRITE_VERSION_HL;
    uint32_t type = SPR_FWD_PARALLEL;
    uint32_t tex_format = SPR_NORMAL;
    uint32_t face_type = SPR_CULL_FRONT;
    uint32_t sync_type = 0;
    float interval = 0.1f;
    int32_t origin_x = 0;
    int32_t origin_y = 0;
    bool auto_origin_center = true;
    int max_palette_colors = 256;
};

struct ConvertResult
{
    bool success = false;
    std::string error;
    std::vector<std::string> output_files;
    std::vector<uint8_t> data;
};

class SpriteConverter
{
public:
    static ConvertResult SprToImage(const std::string& spr_path, const SprToImageParams& params);

    static ConvertResult SprToImageMemory(const uint8_t* spr_data, size_t spr_size, const std::string& name, const SprToImageParams& params);

    static ConvertResult SprFrameToImageMemory(const uint8_t* spr_data, size_t spr_size, int frame_index, ImageFormat format);

    static ConvertResult ImageToSpr(const std::vector<std::string>& image_paths, const std::string& output_path, const ImageToSprParams& params);

    static ConvertResult ImageToSprMemory(const std::vector<const uint8_t*>& image_data_ptrs, const std::vector<size_t>& image_sizes, const ImageToSprParams& params);

    static ConvertResult RGBAToSprMemory(const std::vector<const uint8_t*>& rgba_frames, const std::vector<int>& widths, const std::vector<int>& heights, const ImageToSprParams& params);

    static std::string GetFormatExtension(ImageFormat fmt);
    static ImageFormat DetectFormat(const std::string& path);

private:
    struct QuantizedImage
    {
        std::vector<uint8_t> indices;
        uint8_t palette[768];
        int palette_count = 256;
        int width = 0;
        int height = 0;
    };

    struct ColorBox
    {
        int r_min, r_max;
        int g_min, g_max;
        int b_min, b_max;
        std::vector<int> pixel_indices;

        int LongestAxis() const
        {
            int rr = r_max - r_min;
            int gr = g_max - g_min;
            int br = b_max - b_min;
            if (gr >= rr && gr >= br)
                return 1;
            if (br >= rr && br >= gr) 
                return 2;
            return 0;
        }
    };

    static bool LoadImageRGBA(const std::string& path, std::vector<uint8_t>& rgba, int& w, int& h);

    static bool LoadImageRGBA(const uint8_t* data, size_t size, std::vector<uint8_t>& rgba, int& w, int& h);

public:
    static bool SaveImageRGBA(const std::string& path, const uint8_t* rgba, int w, int h, ImageFormat format);

private:
    static bool EncodeImageToMemory(const uint8_t* rgba, int w, int h, ImageFormat format, std::vector<uint8_t>& out);

    static QuantizedImage QuantizeRGBA(const uint8_t* rgba, int w, int h, int max_colors, uint32_t tex_format);

    static void BuildSharedPalette(const std::vector<const uint8_t*>& rgba_list, const std::vector<int>& widths, const std::vector<int>& heights, int max_colors, uint32_t tex_format, uint8_t* out_palette, int& out_count);

    static void MedianCut(const uint8_t* rgba, int pixel_count, const std::vector<int>& opaque_indices, int max_colors, uint8_t* out_palette, int& out_count);

    static uint8_t FindClosestColor(uint8_t r, uint8_t g, uint8_t b, const uint8_t* palette, int count);

    static bool WriteSprData(const std::vector<QuantizedImage>& frames, const ImageToSprParams& params, const std::vector<std::pair<int32_t, int32_t>>& origins, std::vector<uint8_t>& out);

    static std::string StripExtension(const std::string& filename);
    static std::string GetDirectory(const std::string& path);
    static std::string GetBaseName(const std::string& path);
};