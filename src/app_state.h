#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct AppState
{
    bool sprite_loaded = false;
    std::string filepath;
    std::string fileName;
    int current_frame = 0;
    int total_frames = 0;

    float zoom = 1.0f;
    float scroll_x = 0.0f;
    float scroll_y = 0.0f;

    bool animating = false;
    float anim_speed = 1.0f;
    double anim_time = 0.0;

    bool show_checker = true;
    bool show_properties = true;
    bool show_toolbar = true;
    bool show_about = false;
    bool show_export = false;
    bool show_import = false;

    bool show_progress = false;
    bool progress_done = false;
    bool progress_success = false;
    float progress_value = 0.0f;
    std::string progress_title;
    std::string progress_status;
    std::string progress_result;

    std::string last_dir;
    std::string status_msg;
};