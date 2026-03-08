#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
    #endif

    typedef struct SpriteHandle_s* SpriteHandle;

    typedef struct {
        int32_t  version;
        uint32_t type;
        uint32_t texFormat;
        uint32_t facetype;
        int32_t  bounds[2];
        int32_t  numframes;
        int32_t  num_groups;
        int32_t  palette_colors;
    } SpriteInfo;

    typedef struct {
        int32_t  width;
        int32_t  height;
        int32_t  origin[2];
        float    interval;
        int32_t  group_index;
        int32_t  frame_in_group;
    } FrameInfo;

    typedef struct {
        int32_t  type;
        int32_t  num_frames;
    } GroupInfo;


    SpriteHandle sprite_load_file(const char* filepath);
    SpriteHandle sprite_load_memory(const uint8_t* data, size_t size, const char* name);

    void sprite_free(SpriteHandle handle);

    int sprite_get_info(SpriteHandle handle, SpriteInfo* info);

    int sprite_get_frame_info(SpriteHandle handle, int frame_index, FrameInfo* info);

    int sprite_get_frame_rgba(SpriteHandle handle, int frame_index, uint8_t* out_rgba);

    int sprite_get_group_info(SpriteHandle handle, int group_index, GroupInfo* info);

    int sprite_get_palette(SpriteHandle handle, uint8_t* out_palette_rgb);

    int sprite_export_frame(SpriteHandle handle, int frame_index, const char* output_path, const char* format);

    int sprite_export_all_frames(SpriteHandle handle, const char* output_dir, const char* format, const char* name_pattern);

    #ifdef __cplusplus
}
#endif
