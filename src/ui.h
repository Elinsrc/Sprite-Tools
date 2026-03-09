#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

#include "sprite_loader.h"
#include "renderer.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "ui_utils.h"

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

    void LoadIcons();
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
    bool ImgToolBtn(const char* id, ImGuiImage& img, const char* tip = nullptr, bool active = false, bool enabled = true, float size = 20.0f);
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

    void DrawExportDialog();
    void DrawImportDialog();

    struct ConverterState
    {
        bool show_export = false;
        bool show_import = false;
        int export_format = 0;
        int export_frame = -1;

        std::vector<std::string> import_files;
        int import_version = 2;
        int import_type = 2;
        int import_tex_format = 0;
        float import_interval = 0.1f;
    } m_conv;

    struct TaskState
    {
        std::atomic<bool> running{false};
        std::atomic<bool> done{false};
        std::atomic<bool> cancel_requested{false};
        std::atomic<float> progress{0.0f};
        
        std::mutex mutex;
        std::string title;
        std::string status;      
        std::string result_message;
        bool result_success = false;

        std::string pending_open_file;
        
        std::thread worker;
    } m_task;

    void DrawProgressDialog();
    void StartTask(const std::string& title, std::function<void(TaskState&)> work);

    void StartExport();
    void StartImport();

};