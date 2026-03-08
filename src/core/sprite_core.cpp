#include "sprite_core.h"
#include "sprite_loader.h"
#include <cstring>
#include <cstdio>

struct SpriteHandle_s
{
    SpriteLoader loader;
    std::string  name;
};

extern "C" {

SpriteHandle sprite_load_file(const char* filepath)
{
    if (!filepath) return nullptr;

    auto* h = new (std::nothrow) SpriteHandle_s();
    if (!h) return nullptr;

    h->name = filepath;
    if (!h->loader.Load(filepath)) {
        delete h;
        return nullptr;
    }
    return h;
}

SpriteHandle sprite_load_memory(const uint8_t* data, size_t size, const char* name)
{
    if (!data || size == 0) return nullptr;

    auto* h = new (std::nothrow) SpriteHandle_s();
    if (!h) return nullptr;

    h->name = name ? name : "memory";
    if (!h->loader.LoadFromMemory(data, size, h->name)) {
        delete h;
        return nullptr;
    }
    return h;
}

void sprite_free(SpriteHandle handle)
{
    delete handle;
}

int sprite_get_info(SpriteHandle handle, SpriteInfo* info)
{
    if (!handle || !info) 
        return -1;

    const SpriteData& d = handle->loader.GetData();
    if (!d.loaded) return -1;

    info->version = d.version;
    info->type = d.type;
    info->texFormat = d.texFormat;
    info->facetype = d.facetype;
    info->bounds[0] = d.bounds[0];
    info->bounds[1] = d.bounds[1];
    info->numframes = handle->loader.GetTotalFrameCount();
    info->num_groups = (int32_t)d.groups.size();
    info->palette_colors = d.palette_colors;

    return 0;
}

int sprite_get_frame_info(SpriteHandle handle, int frame_index, FrameInfo* info)
{
    if (!handle || !info) 
        return -1;

    const SpriteData& d = handle->loader.GetData();
    int current = 0;

    for (int gi = 0; gi < (int)d.groups.size(); gi++) {
        for (int fi = 0; fi < (int)d.groups[gi].frames.size(); fi++) {
            if (current == frame_index) 
            {
                const SpriteFrame& f = d.groups[gi].frames[fi];
                info->width = f.width;
                info->height = f.height;
                info->origin[0] = f.origin[0];
                info->origin[1] = f.origin[1];
                info->interval = f.interval;
                info->group_index = gi;
                info->frame_in_group = fi;
                return 0;
            }
            current++;
        }
    }

    return -1;
}

int sprite_get_frame_rgba(SpriteHandle handle, int frame_index, uint8_t* out_rgba)
{
    if (!handle || !out_rgba) 
        return -1;

    SpriteFrame* f = handle->loader.GetFrame(frame_index);
    if (!f || f->rgba.empty()) 
        return -1;

    memcpy(out_rgba, f->rgba.data(), f->rgba.size());
    return 0;
}

int sprite_get_group_info(SpriteHandle handle, int group_index, GroupInfo* info)
{
    if (!handle || !info) 
        return -1;

    const SpriteData& d = handle->loader.GetData();
    if (group_index < 0 || group_index >= (int)d.groups.size()) return -1;

    info->type = d.groups[group_index].type;
    info->num_frames = (int32_t)d.groups[group_index].frames.size();
    return 0;
}

int sprite_get_palette(SpriteHandle handle, uint8_t* out_palette_rgb)
{
    if (!handle || !out_palette_rgb) 
        return 0;

    const SpriteData& d = handle->loader.GetData();
    if (d.palette_colors <= 0) 
        return 0;

    memcpy(out_palette_rgb, d.palette, d.palette_colors * 3);
    return d.palette_colors;
}
}