#include "sprite_converter.h"
#include "sprite_loader.h"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <climits>

#include "stb_image.h"
#include "stb_image_write.h"

std::string SpriteConverter::GetFormatExtension(ImageFormat fmt)
{
    switch (fmt)
    {
        case ImageFormat::PNG: return ".png";
        case ImageFormat::BMP: return ".bmp";
    }
    return ".png";
}

ImageFormat SpriteConverter::DetectFormat(const std::string& path)
{
    std::string ext;
    size_t dot = path.rfind('.');
    if (dot != std::string::npos)
    {
        ext = path.substr(dot);
        for (auto& c : ext)
            c = (char)tolower((unsigned char)c);
    }
    if (ext == ".bmp") 
        return ImageFormat::BMP;
    return ImageFormat::PNG;
}

std::string SpriteConverter::StripExtension(const std::string& filename)
{
    size_t dot = filename.rfind('.');
    size_t slash = filename.find_last_of("/\\");
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
        return filename.substr(0, dot);
    return filename;
}

std::string SpriteConverter::GetDirectory(const std::string& path)
{
    size_t p = path.find_last_of("/\\");
    return (p != std::string::npos) ? path.substr(0, p) : ".";
}

std::string SpriteConverter::GetBaseName(const std::string& path)
{
    size_t p = path.find_last_of("/\\");
    std::string name = (p != std::string::npos) ? path.substr(p + 1) : path;
    return StripExtension(name);
}


bool SpriteConverter::LoadImageRGBA(const std::string& path, std::vector<uint8_t>& rgba, int& w, int& h)
{
    int ch = 0;
    uint8_t* px = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!px) return false;
    rgba.assign(px, px + (size_t)w * h * 4);
    stbi_image_free(px);
    return true;
}

bool SpriteConverter::LoadImageRGBA(const uint8_t* data, size_t size, std::vector<uint8_t>& rgba, int& w, int& h)
{
    int ch = 0;
    uint8_t* px = stbi_load_from_memory(data, (int)size, &w, &h, &ch, 4);
    if (!px) 
        return false;
    rgba.assign(px, px + (size_t)w * h * 4);
    stbi_image_free(px);
    return true;
}

static void stbi_vec_write(void* ctx, void* data, int size)
{
    auto* v = static_cast<std::vector<uint8_t>*>(ctx);
    auto* p = static_cast<const uint8_t*>(data);
    v->insert(v->end(), p, p + size);
}

static std::vector<uint8_t> RGBAtoRGB(const uint8_t* rgba, int w, int h)
{
    size_t pc = (size_t)w * h;
    std::vector<uint8_t> rgb(pc * 3);
    for (size_t i = 0; i < pc; i++)
    {
        rgb[i * 3 + 0] = rgba[i * 4 + 0];
        rgb[i * 3 + 1] = rgba[i * 4 + 1];
        rgb[i * 3 + 2] = rgba[i * 4 + 2];
    }
    return rgb;
}

bool SpriteConverter::SaveImageRGBA(const std::string& path, const uint8_t* rgba, int w, int h, ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::PNG:
            return stbi_write_png(path.c_str(), w, h, 4, rgba, w * 4) != 0;

        case ImageFormat::BMP:
        {
            auto rgb = RGBAtoRGB(rgba, w, h);
            return stbi_write_bmp(path.c_str(), w, h, 3, rgb.data()) != 0;
        }
    }
    return false;
}

bool SpriteConverter::EncodeImageToMemory(const uint8_t* rgba, int w, int h, ImageFormat format, std::vector<uint8_t>& out)
{
    out.clear();

    switch (format)
    {
        case ImageFormat::PNG:
            return stbi_write_png_to_func(stbi_vec_write, &out, w, h, 4, rgba, w * 4) != 0;

        case ImageFormat::BMP:
        {
            auto rgb = RGBAtoRGB(rgba, w, h);
            return stbi_write_bmp_to_func(stbi_vec_write, &out, w, h, 3, rgb.data()) != 0;
        }
    }
    return false;
}

void SpriteConverter::MedianCut(const uint8_t* rgba, int pixel_count, const std::vector<int>& opaque_indices, int max_colors, uint8_t* out_palette, int& out_count)
{
    memset(out_palette, 0, 768);

    if (opaque_indices.empty() || max_colors <= 0)
    {
        for (int i = 0; i < 256; i++)
        {
            out_palette[i * 3 + 0] = (uint8_t)i;
            out_palette[i * 3 + 1] = (uint8_t)i;
            out_palette[i * 3 + 2] = (uint8_t)i;
        }
        out_count = 256;
        return;
    }

    ColorBox initial;
    initial.r_min = initial.g_min = initial.b_min = 255;
    initial.r_max = initial.g_max = initial.b_max = 0;
    initial.pixel_indices = opaque_indices;

    for (int idx : opaque_indices)
    {
        uint8_t r = rgba[idx * 4 + 0];
        uint8_t g = rgba[idx * 4 + 1];
        uint8_t b = rgba[idx * 4 + 2];
        if (r < initial.r_min) initial.r_min = r;
        if (r > initial.r_max) initial.r_max = r;
        if (g < initial.g_min) initial.g_min = g;
        if (g > initial.g_max) initial.g_max = g;
        if (b < initial.b_min) initial.b_min = b;
        if (b > initial.b_max) initial.b_max = b;
    }

    std::vector<ColorBox> boxes;
    boxes.reserve(max_colors);
    boxes.push_back(std::move(initial));

    while ((int)boxes.size() < max_colors)
    {
        int best = -1;
        int best_range = 0;

        for (int i = 0; i < (int)boxes.size(); i++)
        {
            if ((int)boxes[i].pixel_indices.size() < 2)
                continue;

            int rr = boxes[i].r_max - boxes[i].r_min;
            int gr = boxes[i].g_max - boxes[i].g_min;
            int br = boxes[i].b_max - boxes[i].b_min;
            int mx = std::max({rr, gr, br});

            if (mx > best_range)
            {
                best_range = mx;
                best = i;
            }
        }

        if (best < 0) break;

        ColorBox& box = boxes[best];
        int axis = box.LongestAxis();

        std::sort(box.pixel_indices.begin(), box.pixel_indices.end(),
            [&rgba, axis](int a, int b)
            {
                return rgba[a * 4 + axis] < rgba[b * 4 + axis];
            });

        size_t mid = box.pixel_indices.size() / 2;

        ColorBox new_box;
        new_box.pixel_indices.assign(box.pixel_indices.begin() + mid, box.pixel_indices.end());
        box.pixel_indices.resize(mid);

        auto recalc = [&rgba](ColorBox& b)
        {
            b.r_min = b.g_min = b.b_min = 255;
            b.r_max = b.g_max = b.b_max = 0;
            for (int idx : b.pixel_indices)
            {
                uint8_t r = rgba[idx * 4 + 0];
                uint8_t g = rgba[idx * 4 + 1];
                uint8_t bl = rgba[idx * 4 + 2];
                if (r < b.r_min) 
                    b.r_min = r;
                if (r > b.r_max) 
                    b.r_max = r;
                if (g < b.g_min) 
                    b.g_min = g;
                if (g > b.g_max) 
                    b.g_max = g;
                if (bl < b.b_min) 
                    b.b_min = bl;
                if (bl > b.b_max) 
                    b.b_max = bl;
            }
        };

        recalc(box);
        recalc(new_box);
        boxes.push_back(std::move(new_box));
    }

    out_count = (int)boxes.size();
    for (int i = 0; i < out_count; i++)
    {
        int64_t sr = 0, sg = 0, sb = 0;
        int cnt = (int)boxes[i].pixel_indices.size();
        if (cnt == 0) cnt = 1;

        for (int idx : boxes[i].pixel_indices)
        {
            sr += rgba[idx * 4 + 0];
            sg += rgba[idx * 4 + 1];
            sb += rgba[idx * 4 + 2];
        }

        out_palette[i * 3 + 0] = (uint8_t)(sr / cnt);
        out_palette[i * 3 + 1] = (uint8_t)(sg / cnt);
        out_palette[i * 3 + 2] = (uint8_t)(sb / cnt);
    }
}

uint8_t SpriteConverter::FindClosestColor(uint8_t r, uint8_t g, uint8_t b, const uint8_t* palette, int count)
{
    int best = 0;
    int best_dist = INT_MAX;

    for (int i = 0; i < count; i++)
    {
        int dr = (int)r - palette[i * 3 + 0];
        int dg = (int)g - palette[i * 3 + 1];
        int db = (int)b - palette[i * 3 + 2];
        int dist = dr * dr + dg * dg + db * db;

        if (dist < best_dist)
        {
            best_dist = dist;
            best = i;
            if (dist == 0) 
                break;
        }
    }

    return (uint8_t)best;
}

void SpriteConverter::BuildSharedPalette(const std::vector<const uint8_t*>& rgba_list, const std::vector<int>& widths, const std::vector<int>& heights, int max_colors, uint32_t tex_format, uint8_t* out_palette, int& out_count)
{
    size_t total = 0;
    for (size_t i = 0; i < rgba_list.size(); i++)
        total += (size_t)widths[i] * heights[i];

    std::vector<uint8_t> combined(total * 4);
    std::vector<int> opaque;
    opaque.reserve(total);

    size_t offset = 0;
    for (size_t i = 0; i < rgba_list.size(); i++)
    {
        size_t pc = (size_t)widths[i] * heights[i];
        memcpy(combined.data() + offset * 4, rgba_list[i], pc * 4);

        for (size_t j = 0; j < pc; j++)
        {
            uint8_t a = combined[(offset + j) * 4 + 3];
            if (!(tex_format == SPR_ALPHTEST && a < 128))
                opaque.push_back((int)(offset + j));
        }
        offset += pc;
    }

    int pal_slots = max_colors;
    if (tex_format == SPR_ALPHTEST)
        pal_slots = std::min(max_colors - 1, 255);

    MedianCut(combined.data(), (int)total, opaque, pal_slots, out_palette, out_count);

    if (tex_format == SPR_ALPHTEST)
    {
        out_palette[255 * 3 + 0] = 0;
        out_palette[255 * 3 + 1] = 0;
        out_palette[255 * 3 + 2] = 255;
        if (out_count < 256) out_count = 256;
    }

    if (tex_format == SPR_INDEXALPHA && !opaque.empty())
    {
        int64_t sr = 0, sg = 0, sb = 0;
        for (int idx : opaque)
        {
            sr += combined[idx * 4 + 0];
            sg += combined[idx * 4 + 1];
            sb += combined[idx * 4 + 2];
        }
        int cnt = (int)opaque.size();
        uint8_t ar = (uint8_t)(sr / cnt);
        uint8_t ag = (uint8_t)(sg / cnt);
        uint8_t ab = (uint8_t)(sb / cnt);

        for (int i = 0; i < 256; i++)
        {
            float t = (float)i / 255.0f;
            out_palette[i * 3 + 0] = (uint8_t)(ar * t);
            out_palette[i * 3 + 1] = (uint8_t)(ag * t);
            out_palette[i * 3 + 2] = (uint8_t)(ab * t);
        }
        out_count = 256;
    }
}

SpriteConverter::QuantizedImage SpriteConverter::QuantizeRGBA(const uint8_t* rgba, int w, int h, int max_colors, uint32_t tex_format)
{
    QuantizedImage result;
    result.width = w;
    result.height = h;

    int pc = w * h;
    result.indices.resize(pc);

    std::vector<int> opaque;
    for (int i = 0; i < pc; i++)
    {
        uint8_t a = rgba[i * 4 + 3];
        if (!(tex_format == SPR_ALPHTEST && a < 128))
            opaque.push_back(i);
    }

    int pal_slots = max_colors;
    if (tex_format == SPR_ALPHTEST)
        pal_slots = std::min(max_colors - 1, 255);

    MedianCut(rgba, pc, opaque, pal_slots, result.palette, result.palette_count);

    if (tex_format == SPR_ALPHTEST)
    {
        result.palette[255 * 3 + 0] = 0;
        result.palette[255 * 3 + 1] = 0;
        result.palette[255 * 3 + 2] = 255;
        if (result.palette_count < 256)
            result.palette_count = 256;
    }

    for (int i = 0; i < pc; i++)
    {
        uint8_t a = rgba[i * 4 + 3];

        if (tex_format == SPR_ALPHTEST && a < 128)
            result.indices[i] = 255;
        else if (tex_format == SPR_INDEXALPHA)
            result.indices[i] = a;
        else
            result.indices[i] = FindClosestColor(
                rgba[i * 4 + 0], rgba[i * 4 + 1], rgba[i * 4 + 2],
                result.palette, result.palette_count);
    }

    return result;
}

template<typename T>
static void BinWrite(std::vector<uint8_t>& buf, const T& val)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&val);
    buf.insert(buf.end(), p, p + sizeof(T));
}

static void BinWriteBytes(std::vector<uint8_t>& buf,const void* data, size_t count)
{
    const uint8_t* p = static_cast<const uint8_t*>(data);
    buf.insert(buf.end(), p, p + count);
}

bool SpriteConverter::WriteSprData(const std::vector<QuantizedImage>& frames, const ImageToSprParams& params, const std::vector<std::pair<int32_t, int32_t>>& origins, std::vector<uint8_t>& out)
{
    out.clear();
    if (frames.empty()) 
        return false;

    const QuantizedImage& ref = frames[0];

    int32_t max_w = 0, max_h = 0;
    for (const auto& f : frames)
    {
        if (f.width > max_w) max_w = f.width;
        if (f.height > max_h) max_h = f.height;
    }

    float bounding_radius = sqrtf((float)(max_w * max_w + max_h * max_h)) * 0.5f;

    if (params.version == SPRITE_VERSION_HL)
    {
        dsprite_hl_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.ident = IDSPRITEHEADER;
        hdr.version = SPRITE_VERSION_HL;
        hdr.type = params.type;
        hdr.texFormat = params.tex_format;
        hdr.boundingradius = (int32_t)ceilf(bounding_radius);
        hdr.bounds[0] = max_w;
        hdr.bounds[1] = max_h;
        hdr.numframes = (int32_t)frames.size();
        hdr.facetype = params.face_type;
        hdr.synctype = params.sync_type;

        BinWriteBytes(out, &hdr, sizeof(hdr));

        int16_t pal_size = (int16_t)ref.palette_count;
        BinWrite(out, pal_size);
        BinWriteBytes(out, ref.palette, pal_size * 3);
    }
    else if (params.version == SPRITE_VERSION_Q1)
    {
        dsprite_q1_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.ident = IDSPRITEHEADER;
        hdr.version = SPRITE_VERSION_Q1;
        hdr.type = (int32_t)params.type;
        hdr.boundingradius = bounding_radius;
        hdr.bounds[0] = max_w;
        hdr.bounds[1] = max_h;
        hdr.numframes = (int32_t)frames.size();
        hdr.beamlength = 0.0f;
        hdr.synctype = params.sync_type;

        BinWriteBytes(out, &hdr, sizeof(hdr));
    }
    else
    {
        return false;
    }

    for (int i = 0; i < (int)frames.size(); i++)
    {
        const auto& f = frames[i];

        dframetype_t ft;
        ft.type = FRAME_SINGLE;
        BinWrite(out, ft);

        dspriteframe_t fhdr;
        if (i < (int)origins.size())
        {
            fhdr.origin[0] = origins[i].first;
            fhdr.origin[1] = origins[i].second;
        }
        else if (params.auto_origin_center)
        {
            fhdr.origin[0] = -(f.width / 2);
            fhdr.origin[1] = f.height / 2;
        }
        else
        {
            fhdr.origin[0] = params.origin_x;
            fhdr.origin[1] = params.origin_y;
        }

        fhdr.width = f.width;
        fhdr.height = f.height;
        BinWrite(out, fhdr);

        BinWriteBytes(out, f.indices.data(), f.indices.size());
    }

    return true;
}

ConvertResult SpriteConverter::SprToImage(const std::string& spr_path, const SprToImageParams& params)
{
    ConvertResult res;

    SpriteLoader loader;
    if (!loader.Load(spr_path))
    {
        res.error = "Failed to load sprite: " + spr_path;
        return res;
    }

    int total = loader.GetTotalFrameCount();
    if (total <= 0)
    {
        res.error = "Sprite has no frames";
        return res;
    }

    std::string prefix = params.output_prefix;
    if (prefix.empty())
        prefix = GetBaseName(spr_path);

    std::string dir = params.output_dir;
    if (dir.empty())
        dir = GetDirectory(spr_path);

    std::string ext = GetFormatExtension(params.format);

    int start = 0, end_idx = total;
    if (params.frame_index >= 0 && params.frame_index < total)
    {
        start = params.frame_index;
        end_idx = params.frame_index + 1;
    }

    for (int i = start; i < end_idx; i++)
    {
        SpriteFrame* frame = loader.GetFrame(i);
        if (!frame || frame->rgba.empty())
        {
            res.error = "Failed to get frame " + std::to_string(i);
            return res;
        }

        char fname[512];
        if (total == 1 && params.frame_index < 0)
            snprintf(fname, sizeof(fname), "%s/%s%s", dir.c_str(), prefix.c_str(), ext.c_str());
        else
            snprintf(fname, sizeof(fname), "%s/%s_%03d%s", dir.c_str(), prefix.c_str(), i, ext.c_str());

        if (!SaveImageRGBA(fname, frame->rgba.data(),
            frame->width, frame->height, params.format))
        {
            res.error = "Failed to save: " + std::string(fname);
            return res;
        }

        res.output_files.push_back(fname);
    }

    res.success = true;
    return res;
}

ConvertResult SpriteConverter::SprToImageMemory(const uint8_t* spr_data, size_t spr_size, const std::string& name, const SprToImageParams& params)
{
    ConvertResult res;

    SpriteLoader loader;
    if (!loader.LoadFromMemory(spr_data, spr_size, name))
    {
        res.error = "Failed to load sprite from memory";
        return res;
    }

    int total = loader.GetTotalFrameCount();
    if (total <= 0)
    {
        res.error = "Sprite has no frames";
        return res;
    }

    std::string prefix = params.output_prefix.empty() ? "sprite" : params.output_prefix;
    std::string dir = params.output_dir.empty() ? "." : params.output_dir;
    std::string ext = GetFormatExtension(params.format);

    int start = 0, end_idx = total;
    if (params.frame_index >= 0 && params.frame_index < total)
    {
        start = params.frame_index;
        end_idx = params.frame_index + 1;
    }

    for (int i = start; i < end_idx; i++)
    {
        SpriteFrame* frame = loader.GetFrame(i);
        if (!frame || frame->rgba.empty())
        {
            res.error = "Failed to get frame " + std::to_string(i);
            return res;
        }

        char fname[512];
        if (total == 1 && params.frame_index < 0)
            snprintf(fname, sizeof(fname), "%s/%s%s", dir.c_str(), prefix.c_str(), ext.c_str());
        else
            snprintf(fname, sizeof(fname), "%s/%s_%03d%s", dir.c_str(), prefix.c_str(), i, ext.c_str());

        if (!SaveImageRGBA(fname, frame->rgba.data(),
            frame->width, frame->height, params.format))
        {
            res.error = "Failed to save: " + std::string(fname);
            return res;
        }

        res.output_files.push_back(fname);
    }

    res.success = true;
    return res;
}

ConvertResult SpriteConverter::SprFrameToImageMemory(const uint8_t* spr_data, size_t spr_size, int frame_index, ImageFormat format)
{
    ConvertResult res;

    SpriteLoader loader;
    if (!loader.LoadFromMemory(spr_data, spr_size, "memory"))
    {
        res.error = "Failed to load sprite";
        return res;
    }

    SpriteFrame* frame = loader.GetFrame(frame_index);
    if (!frame || frame->rgba.empty())
    {
        res.error = "Invalid frame index: " + std::to_string(frame_index);
        return res;
    }

    if (!EncodeImageToMemory(frame->rgba.data(),
        frame->width, frame->height, format, res.data))
    {
        res.error = "Failed to encode image";
        return res;
    }

    res.success = true;
    return res;
}

ConvertResult SpriteConverter::ImageToSpr(const std::vector<std::string>& image_paths, const std::string& output_path, const ImageToSprParams& params)
{
    ConvertResult res;

    if (image_paths.empty())
    {
        res.error = "No input images";
        return res;
    }

    std::vector<std::vector<uint8_t>> rgba_storage;
    std::vector<const uint8_t*> rgba_ptrs;
    std::vector<int> widths, heights;

    for (const auto& path : image_paths)
    {
        std::vector<uint8_t> rgba;
        int w, h;
        if (!LoadImageRGBA(path, rgba, w, h))
        {
            res.error = "Failed to load: " + path;
            return res;
        }

        if (w > 4096 || h > 4096 || w <= 0 || h <= 0)
        {
            res.error = "Bad size in " + path + ": " + std::to_string(w) + "x" + std::to_string(h);
            return res;
        }

        widths.push_back(w);
        heights.push_back(h);
        rgba_storage.push_back(std::move(rgba));
    }

    for (auto& v : rgba_storage)
        rgba_ptrs.push_back(v.data());

    uint8_t palette[768];
    int pal_count = 0;

    BuildSharedPalette(rgba_ptrs, widths, heights, params.max_palette_colors, params.tex_format, palette, pal_count);

    std::vector<QuantizedImage> frames;
    std::vector<std::pair<int32_t, int32_t>> origins;

    for (size_t i = 0; i < rgba_ptrs.size(); i++)
    {
        int w = widths[i], h = heights[i];
        int pc = w * h;

        QuantizedImage qi;
        qi.width = w;
        qi.height = h;
        qi.indices.resize(pc);
        memcpy(qi.palette, palette, 768);
        qi.palette_count = pal_count;

        for (int j = 0; j < pc; j++)
        {
            const uint8_t* px = rgba_ptrs[i] + j * 4;
            uint8_t a = px[3];

            if (params.tex_format == SPR_ALPHTEST && a < 128)
                qi.indices[j] = 255;
            else if (params.tex_format == SPR_INDEXALPHA)
                qi.indices[j] = a;
            else
                qi.indices[j] = FindClosestColor(px[0], px[1], px[2], palette, pal_count);
        }

        frames.push_back(std::move(qi));

        if (params.auto_origin_center)
            origins.push_back({-(w / 2), h / 2});
        else
            origins.push_back({params.origin_x, params.origin_y});
    }

    std::vector<uint8_t> spr_data;
    if (!WriteSprData(frames, params, origins, spr_data))
    {
        res.error = "Failed to build SPR data";
        return res;
    }

    std::ofstream file(output_path, std::ios::binary);
    if (!file.is_open())
    {
        res.error = "Failed to create: " + output_path;
        return res;
    }

    file.write(reinterpret_cast<const char*>(spr_data.data()), (std::streamsize)spr_data.size());
    file.close();

    res.success = true;
    res.output_files.push_back(output_path);
    return res;
}

ConvertResult SpriteConverter::ImageToSprMemory(const std::vector<const uint8_t*>& image_data_ptrs, const std::vector<size_t>& image_sizes, const ImageToSprParams& params)
{
    ConvertResult res;

    if (image_data_ptrs.empty() || image_data_ptrs.size() != image_sizes.size())
    {
        res.error = "Invalid input";
        return res;
    }

    std::vector<std::vector<uint8_t>> rgba_storage;
    std::vector<const uint8_t*> rgba_ptrs;
    std::vector<int> widths, heights;

    for (size_t i = 0; i < image_data_ptrs.size(); i++)
    {
        std::vector<uint8_t> rgba;
        int w, h;
        if (!LoadImageRGBA(image_data_ptrs[i], image_sizes[i], rgba, w, h))
        {
            res.error = "Failed to decode image " + std::to_string(i);
            return res;
        }

        widths.push_back(w);
        heights.push_back(h);
        rgba_storage.push_back(std::move(rgba));
    }

    for (auto& v : rgba_storage)
        rgba_ptrs.push_back(v.data());

    return RGBAToSprMemory(rgba_ptrs, widths, heights, params);
}

ConvertResult SpriteConverter::RGBAToSprMemory(const std::vector<const uint8_t*>& rgba_frames, const std::vector<int>& widths, const std::vector<int>& heights, const ImageToSprParams& params)
{
    ConvertResult res;

    if (rgba_frames.empty() || rgba_frames.size() != widths.size() || rgba_frames.size() != heights.size())
    {
        res.error = "Invalid input arrays";
        return res;
    }

    uint8_t palette[768];
    int pal_count = 0;

    BuildSharedPalette(rgba_frames, widths, heights, params.max_palette_colors, params.tex_format, palette, pal_count);

    std::vector<QuantizedImage> frames;
    std::vector<std::pair<int32_t, int32_t>> origins;

    for (size_t i = 0; i < rgba_frames.size(); i++)
    {
        int w = widths[i], h = heights[i];
        int pc = w * h;

        QuantizedImage qi;
        qi.width = w;
        qi.height = h;
        qi.indices.resize(pc);
        memcpy(qi.palette, palette, 768);
        qi.palette_count = pal_count;

        for (int j = 0; j < pc; j++)
        {
            const uint8_t* px = rgba_frames[i] + j * 4;
            uint8_t a = px[3];

            if (params.tex_format == SPR_ALPHTEST && a < 128)
                qi.indices[j] = 255;
            else if (params.tex_format == SPR_INDEXALPHA)
                qi.indices[j] = a;
            else
                qi.indices[j] = FindClosestColor(px[0], px[1], px[2], palette, pal_count);
        }

        frames.push_back(std::move(qi));

        if (params.auto_origin_center)
            origins.push_back({-(w / 2), h / 2});
        else
            origins.push_back({params.origin_x, params.origin_y});
    }

    if (!WriteSprData(frames, params, origins, res.data))
    {
        res.error = "Failed to build SPR data";
        return res;
    }

    res.success = true;
    return res;
}