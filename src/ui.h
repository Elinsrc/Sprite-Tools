#pragma once

#include <string>
#include <chrono>

#include "sprite_loader.h"
#include "renderer.h"

#include "imgui.h"
#include "imgui_internal.h"

struct AppState
{
    SpriteLoader loader;
    SpriteRenderer renderer;

    bool sprite_loaded = false;
    int current_frame = 0;
    int total_frames = 0;

    bool animating = false;
    float anim_speed = 1.0f;
    double anim_time = 0.0;
    double last_time = 0.0;

    float zoom = 1.0f;
    bool show_checker = true;
    bool show_info = true;
    bool show_toolbar = true;
    bool show_about = false;

    float scroll_x = 0.0f;
    float scroll_y = 0.0f;
    bool dragging = false;
    ImVec2 drag_start = ImVec2(0, 0);
    float drag_sx = 0.0f;
    float drag_sy = 0.0f;

    std::string pending_file;
    std::string status_msg;
    double status_time = 0.0;
    std::string last_dir;

    bool request_exit = false;
    std::string window_title;
    bool title_changed = false;
};

class UI
{
public:
    UI() = default;
    ~UI() = default;

    void SetupTheme();
    void RenderFrame();

    void SetPendingFile(const std::string& path);
    void ProcessPendingFile();

    bool IsExitRequested() const { return m_app.request_exit; }
    bool IsTitleChanged() const { return m_app.title_changed; }
    std::string ConsumeTitle();

    bool IsSpriteLoaded() const { return m_app.sprite_loaded; }
    void CleanupResources();

    AppState& GetState() { return m_app; }
    const AppState& GetState() const { return m_app; }

private:
    void Tooltip(const char* text);
    bool ToolBtn(const char* label, const char* tip, bool active = false, bool enabled = true);
    void ToolSep();
    void Section(const char* text);
    void PropRow(const char* label, const char* fmt, ...);

    void SetStatus(const std::string& msg);
    void OpenFileDialog();
    void LoadSpriteFile(const std::string& path);
    void CloseSprite();

    void DrawMenuBar();
    void DrawToolbar();
    void DrawChecker(ImDrawList* dl, ImVec2 pos, ImVec2 sz, float cell = 12.0f);
    void DrawViewport();
    void DrawProperties();
    void DrawStatusBar();
    void DrawAbout();
    void HandleKeys();

    static std::string GetDir(const std::string& path);
    static std::string GetFilename(const std::string& path);

    AppState m_app;
};