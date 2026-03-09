#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int sprite_export_frames(const char* spr_path, const char* output_dir, const char* prefix, int format, int frame_index);

int sprite_export_frame_to_buffer(const uint8_t* spr_data, size_t spr_size, int frame_index, int format, uint8_t** out_data, size_t* out_size);

int sprite_create_from_files(const char** image_paths, int image_count, const char* output_path, int version, int type, int tex_format, float interval);

int sprite_create_from_buffers(const uint8_t** image_data_ptrs, const size_t* image_sizes, int image_count, int version, int type, int tex_format, float interval, uint8_t** out_spr_data, size_t* out_spr_size);

void sprite_free_buffer(uint8_t* buffer);

const char* sprite_converter_last_error(void);

#ifdef __cplusplus
}
#endif