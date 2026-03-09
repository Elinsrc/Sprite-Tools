#include "ui.h"
#include "sprite_converter.h"
#include "icons.h"

ImGuiImage m_pCenterFocus;
ImGuiImage m_pFolder;
ImGuiImage m_pNavigateBefore;
ImGuiImage m_pNavigateNext;
ImGuiImage m_pPause;
ImGuiImage m_pPlay;
ImGuiImage m_pSkipNext;
ImGuiImage m_pSkipPrevious;
ImGuiImage m_pZoomIn;
ImGuiImage m_pZoomOut;

#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <algorithm>
#include <fstream>

#include "portable-file-dialogs.h"
#include "stb_image.h"

void UI::LoadIcons()
{
    m_pCenterFocus = UI_utils::LoadImageFromMemory(center_focus_strong_png, center_focus_strong_png_len);
    m_pFolder = UI_utils::LoadImageFromMemory(folder_png, folder_png_len);
    m_pNavigateBefore = UI_utils::LoadImageFromMemory(navigate_before_png, navigate_before_png_len);
    m_pNavigateNext = UI_utils::LoadImageFromMemory(navigate_next_png, navigate_next_png_len);
    m_pPause = UI_utils::LoadImageFromMemory(pause_png, pause_png_len);
    m_pPlay = UI_utils::LoadImageFromMemory(play_png, play_png_len);
    m_pSkipNext= UI_utils::LoadImageFromMemory(skip_next_png, skip_next_png_len);
    m_pSkipPrevious = UI_utils::LoadImageFromMemory(skip_previous_png, skip_previous_png_len);
    m_pZoomIn = UI_utils::LoadImageFromMemory(zoom_in_png, zoom_in_png_len);
    m_pZoomOut = UI_utils::LoadImageFromMemory(zoom_out_png, zoom_out_png_len);
}

void UI::SetupTheme()
{
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 4.0f;
    s.FrameRounding = 3.0f;
    s.GrabRounding = 3.0f;
    s.TabRounding = 3.0f;
    s.ScrollbarRounding = 4.0f;
    s.PopupRounding = 4.0f;
    s.ChildRounding = 3.0f;

    s.WindowPadding = ImVec2(8, 8);
    s.FramePadding = ImVec2(6, 3);
    s.ItemSpacing = ImVec2(6, 4);
    s.ItemInnerSpacing = ImVec2(4, 4);
    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.PopupBorderSize = 1.0f;
    s.ScrollbarSize = 12.0f;
    s.GrabMinSize = 10.0f;
    s.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    ImVec4* c = s.Colors;

    ImVec4 bg_dark = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
    ImVec4 bg_mid = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
    ImVec4 bg_light = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
    ImVec4 bg_lighter = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);

    ImVec4 accent = ImVec4(0.40f, 0.60f, 0.95f, 1.00f);
    ImVec4 accent_dim = ImVec4(0.30f, 0.45f, 0.75f, 1.00f);
    ImVec4 accent_lit = ImVec4(0.55f, 0.72f, 1.00f, 1.00f);

    ImVec4 text = ImVec4(0.88f, 0.88f, 0.92f, 1.00f);
    ImVec4 text_dim = ImVec4(0.50f, 0.50f, 0.58f, 1.00f);

    c[ImGuiCol_WindowBg] = bg_dark;
    c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.16f, 0.97f);
    c[ImGuiCol_MenuBarBg] = bg_mid;

    c[ImGuiCol_TitleBg] = bg_dark;
    c[ImGuiCol_TitleBgActive] = bg_mid;
    c[ImGuiCol_TitleBgCollapsed] = bg_dark;

    c[ImGuiCol_Border] = ImVec4(0.28f, 0.28f, 0.35f, 0.50f);
    c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    c[ImGuiCol_FrameBg] = bg_light;
    c[ImGuiCol_FrameBgHovered] = bg_lighter;
    c[ImGuiCol_FrameBgActive] = ImVec4(accent.x, accent.y, accent.z, 0.30f);

    c[ImGuiCol_Button] = bg_lighter;
    c[ImGuiCol_ButtonHovered] = ImVec4(accent.x, accent.y, accent.z, 0.45f);
    c[ImGuiCol_ButtonActive] = ImVec4(accent.x, accent.y, accent.z, 0.65f);

    c[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.18f);
    c[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
    c[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 0.50f);

    c[ImGuiCol_Tab] = bg_mid;
    c[ImGuiCol_TabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.50f);
    c[ImGuiCol_TabActive] = accent_dim;
    c[ImGuiCol_TabUnfocused] = bg_dark;
    c[ImGuiCol_TabUnfocusedActive] = bg_mid;

    c[ImGuiCol_SliderGrab] = accent_dim;
    c[ImGuiCol_SliderGrabActive] = accent;

    c[ImGuiCol_CheckMark] = accent_lit;

    c[ImGuiCol_ScrollbarBg] = bg_dark;
    c[ImGuiCol_ScrollbarGrab] = ImVec4(0.32f, 0.32f, 0.40f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.42f, 0.52f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive] = accent_dim;

    c[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.35f, 0.60f);
    c[ImGuiCol_SeparatorHovered] = accent_dim;
    c[ImGuiCol_SeparatorActive] = accent;

    c[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.35f, 0.30f);
    c[ImGuiCol_ResizeGripHovered] = accent_dim;
    c[ImGuiCol_ResizeGripActive] = accent;

    c[ImGuiCol_Text] = text;
    c[ImGuiCol_TextDisabled] = text_dim;
    c[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.30f);

    c[ImGuiCol_DragDropTarget] = accent_lit;
    c[ImGuiCol_NavHighlight] = accent;
    c[ImGuiCol_PlotHistogram] = accent;
    c[ImGuiCol_PlotHistogramHovered] = accent_lit;
}

void UI::Tooltip(const char* text)
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(text);
        ImGui::EndTooltip();
        ImGui::PopStyleVar();
    }
}

bool UI::ImgToolBtn(const char* id, ImGuiImage& img, const char* tip, bool active, bool enabled, float size)
{
    if (!enabled)
        ImGui::BeginDisabled();

    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.40f, 0.60f, 0.95f, 0.70f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.68f, 1.00f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.72f, 1.00f, 0.90f));
    }

    ImVec2 btn_size(size + 8, size + 8);
    bool pressed = ImGui::ImageButton(id, (ImTextureID)(intptr_t)img.texture, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));

    if (tip)
        Tooltip(tip);

    if (active)
        ImGui::PopStyleColor(3);

    if (!enabled)
        ImGui::EndDisabled();

    return pressed;
}

void UI::ToolSep()
{
    ImGui::SameLine(0, 2);
    ImVec2 p = ImGui::GetCursorScreenPos();
    float h = ImGui::GetFrameHeight();
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + 1, p.y + 3), ImVec2(p.x + 1, p.y + h - 3), IM_COL32(255, 255, 255, 30), 1.0f);
    ImGui::Dummy(ImVec2(4, h));
    ImGui::SameLine(0, 2);
}

void UI::Section(const char* text)
{
    ImGui::Spacing();
    ImGui::Spacing();
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 p = ImGui::GetCursorScreenPos();
    float text_w = ImGui::CalcTextSize(text).x;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    float y = p.y + ImGui::GetTextLineHeight() * 0.5f;

    dl->AddLine(ImVec2(p.x, y), ImVec2(p.x + 6, y), IM_COL32(100, 100, 120, 100), 1.0f);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.65f, 0.90f, 1.0f));
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();

    float after_text = p.x + 10 + text_w + 6;
    dl->AddLine(ImVec2(after_text, y), ImVec2(p.x + w, y), IM_COL32(100, 100, 120, 100), 1.0f);

    ImGui::Spacing();
}

void UI::PropRow(const char* label, const char* fmt, ...)
{
    float label_w = 110.0f;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.62f, 1.0f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    ImGui::SameLine(label_w);

    va_list args;
    va_start(args, fmt);
    ImGui::TextV(fmt, args);
    va_end(args);
}

std::string UI::GetDir(const std::string& path)
{
    size_t p = path.find_last_of("/\\");
    return (p != std::string::npos) ? path.substr(0, p) : "";
}

std::string UI::GetFilename(const std::string& path)
{
    size_t p = path.find_last_of("/\\");
    return (p != std::string::npos) ? path.substr(p + 1) : path;
}

void UI::SetStatus(const std::string& msg)
{
    m_app.status_msg = msg;
    auto now = std::chrono::high_resolution_clock::now();
    m_app.status_time = std::chrono::duration<double>(now.time_since_epoch()).count();
}

void UI::OpenFileDialog()
{
    auto sel = pfd::open_file("Open Sprite", m_app.last_dir, { "Sprite files (*.spr)", "*.spr", "All files", "*" }, pfd::opt::none).result();

    if (!sel.empty())
        m_app.pending_file = sel[0];
}

void UI::LoadSpriteFile(const std::string& path)
{
    if (m_app.sprite_loaded)
    {
        m_app.renderer.DeleteTextures(m_app.loader.GetData());
        m_app.loader.Unload();
        m_app.sprite_loaded = false;
    }

    if (m_app.loader.Load(path))
    {
        m_app.renderer.UploadAllFrames(m_app.loader.GetData());
        m_app.sprite_loaded = true;
        m_app.current_frame = 0;
        m_app.total_frames = m_app.loader.GetTotalFrameCount();
        m_app.animating = false;
        m_app.anim_time = 0.0;
        m_app.scroll_x = m_app.scroll_y = 0;
        m_app.last_dir = GetDir(path);

        SpriteFrame* f = m_app.loader.GetFrame(0);
        if (f)
        {
            float m = (float)std::max(f->width, f->height);
            m_app.zoom = (m < 64) ? 4.0f : (m < 128) ? 2.0f : 1.0f;
        }

        m_app.window_title = "Sprite-Tools — " + GetFilename(path);
        m_app.title_changed = true;

        SetStatus("Loaded: " + GetFilename(path) + " (" + std::to_string(m_app.total_frames) + " frames)");
    }
    else
    {
        SetStatus("Failed to load: " + GetFilename(path));
    }
}

void UI::CloseSprite()
{
    if (!m_app.sprite_loaded) 
        return;

    m_app.renderer.DeleteTextures(m_app.loader.GetData());
    m_app.loader.Unload();
    m_app.sprite_loaded = false;
    m_app.current_frame = 0;
    m_app.total_frames  = 0;

    m_app.window_title = "Sprite-Tools";
    m_app.title_changed = true;

    SetStatus("Closed");
}

void UI::SetPendingFile(const std::string& path)
{
    m_app.pending_file = path;
}

void UI::ProcessPendingFile()
{
    if (m_app.pending_file.empty()) 
        return;

    LoadSpriteFile(m_app.pending_file);
    m_app.pending_file.clear();
}

std::string UI::ConsumeTitle()
{
    m_app.title_changed = false;
    return m_app.window_title;
}

void UI::CleanupResources()
{
    if (m_app.sprite_loaded)
        m_app.renderer.DeleteTextures(m_app.loader.GetData());
}

void UI::DrawMenuBar()
{
    if (!ImGui::BeginMainMenuBar()) 
        return;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open", "Ctrl+O"))
            OpenFileDialog();

        if (ImGui::MenuItem("Close", nullptr, false, m_app.sprite_loaded))
            CloseSprite();
        
        ImGui::Separator();

        if (ImGui::MenuItem("Export Frames...", "Ctrl+E",
            false, m_app.sprite_loaded))
            m_conv.show_export = true;

        if (ImGui::MenuItem("Import Images to SPR...", "Ctrl+I"))
            m_conv.show_import = true;

        ImGui::Separator();

        if (ImGui::MenuItem("Exit", "Alt+F4"))
            m_app.request_exit = true;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Toolbar",    nullptr, &m_app.show_toolbar);
        ImGui::MenuItem("Properties", nullptr, &m_app.show_info);
        ImGui::Separator();
        ImGui::MenuItem("Transparency grid", nullptr, &m_app.show_checker);

        ImGui::Separator();

        if (ImGui::BeginMenu("Zoom"))
        {
            const float zooms[]  = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f };
            const char* labels[] = { "25%", "50%", "100%", "200%", "400%", "800%" };
            for (int i = 0; i < 6; i++)
            {
                bool sel = (std::abs(m_app.zoom - zooms[i]) < 0.01f);
                if (ImGui::MenuItem(labels[i], nullptr, sel))
                {
                    m_app.zoom = zooms[i];
                    m_app.scroll_x = m_app.scroll_y = 0;
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    bool can_play = m_app.sprite_loaded && m_app.total_frames > 1;
    if (ImGui::BeginMenu("Playback", can_play))
    {
        if (ImGui::MenuItem(m_app.animating ? "Pause" : "Play", "Space"))
        {
            m_app.animating = !m_app.animating;
            if (m_app.animating)
            {
                auto now = std::chrono::high_resolution_clock::now();
                m_app.last_time = std::chrono::duration<double>(
                    now.time_since_epoch()).count();
            }
        }
        ImGui::Separator();

        if (ImGui::MenuItem("First", "Home"))
            m_app.current_frame = 0;
        if (ImGui::MenuItem("Previous", "Left"))
            m_app.current_frame = (m_app.current_frame - 1 + m_app.total_frames) % m_app.total_frames;
        if (ImGui::MenuItem("Next", "Right"))
            m_app.current_frame = (m_app.current_frame + 1) % m_app.total_frames;
        if (ImGui::MenuItem("Last", "End"))
            m_app.current_frame = m_app.total_frames - 1;

        ImGui::Separator();
        ImGui::Text("Speed:");
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##spd", &m_app.anim_speed, 0.1f, 8.0f, "%.1fx");

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About"))
            m_app.show_about = true;
        ImGui::EndMenu();
    }

    if (m_app.sprite_loaded)
    {
        SpriteFrame* f = m_app.loader.GetFrame(m_app.current_frame);
        if (f)
        {
            char buf[96];
            snprintf(buf, sizeof(buf), "%dx%d  |  %d/%d  |  %.0f%%", f->width, f->height, m_app.current_frame + 1, m_app.total_frames, m_app.zoom * 100.0f);
            float tw = ImGui::CalcTextSize(buf).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - tw - 16);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.50f, 0.58f, 1.0f));
            ImGui::TextUnformatted(buf);
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndMainMenuBar();
}

void UI::DrawToolbar()
{
    if (!m_app.show_toolbar)
        return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    float h = 38.0f;
    float iconSize = 20.0f;

    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.13f, 0.16f, 1.0f));

    ImGui::Begin("##tb", nullptr,
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings);

    bool loaded = m_app.sprite_loaded;
    bool multi  = loaded && m_app.total_frames > 1;

    if (ImgToolBtn("##open", m_pFolder, "Open (Ctrl+O)"))
        OpenFileDialog();
    ImGui::SameLine();

    ToolSep();

    if (ImgToolBtn("##first", m_pSkipPrevious, "First (Home)", false, multi, iconSize))
        m_app.current_frame = 0;
    ImGui::SameLine();

    if (ImgToolBtn("##prev", m_pNavigateBefore, "Previous (Left)", false, multi, iconSize))
        m_app.current_frame = (m_app.current_frame - 1 + std::max(1, m_app.total_frames)) % std::max(1, m_app.total_frames);
    ImGui::SameLine();
    {
        bool playing = m_app.animating;
        ImGuiImage& playIcon = playing ? m_pPause : m_pPlay;
        if (ImgToolBtn("##playpause", playIcon, "Play/Pause (Space)", playing, multi, iconSize))
        {
            m_app.animating = !m_app.animating;
            if (m_app.animating)
            {
                auto now = std::chrono::high_resolution_clock::now();
                m_app.last_time = std::chrono::duration<double>(now.time_since_epoch()).count();
            }
        }
    }
    ImGui::SameLine();

    if (ImgToolBtn("##next", m_pNavigateNext, "Next (Right)", false, multi, iconSize))
        m_app.current_frame = (m_app.current_frame + 1) % std::max(1, m_app.total_frames);
    ImGui::SameLine();

    if (ImgToolBtn("##last", m_pSkipNext, "Last (End)", false, multi, iconSize))
        m_app.current_frame = std::max(0, m_app.total_frames - 1);

    ImGui::SameLine();
    ToolSep();

    ImGui::BeginDisabled(!multi);
    ImGui::SetNextItemWidth(70);
    ImGui::SliderFloat("##spd", &m_app.anim_speed, 0.1f, 8.0f, "%.1fx");
    Tooltip("Playback speed");
    ImGui::EndDisabled();

    ImGui::SameLine();
    ToolSep();

    if (ImgToolBtn("##zoomout", m_pZoomOut, "Zoom out (-)", false, true, iconSize))
    {
        m_app.zoom = std::max(0.25f, m_app.zoom * 0.5f);
        m_app.scroll_x = m_app.scroll_y = 0;
    }
    ImGui::SameLine();

    ImGui::SetNextItemWidth(70);
    float zoom_pct = m_app.zoom * 100.0f;
    if (ImGui::SliderFloat("##zm", &zoom_pct, 25, 1600, "%.0f%%", ImGuiSliderFlags_Logarithmic))
    {
        m_app.zoom = zoom_pct / 100.0f;
    }
    Tooltip("Zoom (Ctrl+Wheel)");
    ImGui::SameLine();

    if (ImgToolBtn("##zoomin", m_pZoomIn, "Zoom in (+)", false, true, iconSize))
        m_app.zoom = std::min(16.0f, m_app.zoom * 2.0f);
    ImGui::SameLine();

    if (ImgToolBtn("##center", m_pCenterFocus, "100% (1)", false, true, iconSize))
    {
        m_app.zoom = 1.0f;
        m_app.scroll_x = m_app.scroll_y = 0;
    }

    if (multi)
    {
        ImGui::SameLine();
        ToolSep();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.62f, 1.0f));
        ImGui::Text("Frame");
        ImGui::PopStyleColor();
        ImGui::SameLine();

        float remaining = ImGui::GetContentRegionAvail().x - 4;
        ImGui::SetNextItemWidth(std::max(60.0f, remaining));
        int f = m_app.current_frame;
        char frame_label[32];
        snprintf(frame_label, sizeof(frame_label), "%d / %d", f + 1, m_app.total_frames);
        if (ImGui::SliderInt("##fr", &f, 0, m_app.total_frames - 1, frame_label, ImGuiSliderFlags_AlwaysClamp))
            m_app.current_frame = f;
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void UI::DrawChecker(ImDrawList* dl, ImVec2 pos, ImVec2 sz, float cell)
{
    ImU32 a = IM_COL32(160, 160, 160, 255);
    ImU32 b = IM_COL32(110, 110, 110, 255);
    int nx = (int)(sz.x / cell) + 1;
    int ny = (int)(sz.y / cell) + 1;
    for (int y = 0; y < ny; y++)
        for (int x = 0; x < nx; x++)
        {
            float x0 = pos.x + x * cell;
            float y0 = pos.y + y * cell;
            dl->AddRectFilled(ImVec2(x0, y0), ImVec2(std::min(x0 + cell, pos.x + sz.x), std::min(y0 + cell, pos.y + sz.y)), ((x + y) & 1) ? b : a);
        }
}

void UI::DrawViewport()
{
    float tb_h = m_app.show_toolbar ? 38.0f : 0.0f;
    float sb_h = 26.0f;
    float info_w = m_app.show_info ? 300.0f : 0.0f;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    float vw = vp->WorkSize.x - info_w;
    float vh = vp->WorkSize.y - tb_h - sb_h;

    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y + tb_h));
    ImGui::SetNextWindowSize(ImVec2(vw, vh));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.14f, 1.0f));

    ImGui::Begin("##viewport", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    if (!m_app.sprite_loaded)
    {
        ImVec2 ws = ImGui::GetWindowSize();

        const char* t1 = "Sprite-Tools";

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.55f, 0.88f, 0.80f));
        float old_scale = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale = 1.5f;
        ImGui::PushFont(ImGui::GetFont());
        ImVec2 ts_big = ImGui::CalcTextSize(t1);
        ImGui::SetCursorPos(ImVec2((ws.x - ts_big.x) * 0.5f, ws.y * 0.36f));
        ImGui::TextUnformatted(t1);
        ImGui::GetFont()->Scale = old_scale;
        ImGui::PopFont();
        ImGui::PopStyleColor();

        const char* t2 = "Drop .spr file here or press Ctrl+O";
        ImVec2 ts2 = ImGui::CalcTextSize(t2);
        ImGui::SetCursorPos(ImVec2((ws.x - ts2.x) * 0.5f, ws.y * 0.36f + ts_big.y + 12));
        ImGui::TextDisabled("%s", t2);

        ImGui::End();
        return;
    }

    SpriteFrame* frame = m_app.loader.GetFrame(m_app.current_frame);
    if (!frame)
    {
        ImGui::End();
        return;
    }

    if (m_app.animating && m_app.total_frames > 1)
    {
        auto now = std::chrono::high_resolution_clock::now();
        double t = std::chrono::duration<double>(now.time_since_epoch()).count();
        m_app.anim_time += (t - m_app.last_time) * m_app.anim_speed;
        m_app.last_time = t;
        float iv = frame->interval > 0.0f ? frame->interval : 0.1f;
        while (m_app.anim_time >= iv)
        {
            m_app.anim_time -= iv;
            m_app.current_frame = (m_app.current_frame + 1) % m_app.total_frames;
            frame = m_app.loader.GetFrame(m_app.current_frame);
            if (!frame) { ImGui::End(); return; }
            iv = frame->interval > 0.0f ? frame->interval : 0.1f;
        }
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float iw = frame->width * m_app.zoom;
    float ih = frame->height * m_app.zoom;

    float sbt = 12.0f;
    bool sx = iw > avail.x;
    bool sy = ih > avail.y;
    float cw = avail.x - (sy ? sbt : 0);
    float ch = avail.y - (sx ? sbt : 0);
    sx = iw > cw; sy = ih > ch;
    cw = avail.x - (sy ? sbt : 0);
    ch = avail.y - (sx ? sbt : 0);

    float msx = std::max(0.0f, iw - cw);
    float msy = std::max(0.0f, ih - ch);

    if (ImGui::IsWindowHovered())
    {
        float wh = ImGui::GetIO().MouseWheel;
        if (wh != 0)
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                float oz = m_app.zoom;
                m_app.zoom = std::clamp(m_app.zoom * (1.0f + wh * 0.15f), 0.25f, 16.0f);
                float r = m_app.zoom / oz;
                m_app.scroll_x = (m_app.scroll_x + cw * 0.5f) * r - cw * 0.5f;
                m_app.scroll_y = (m_app.scroll_y + ch * 0.5f) * r - ch * 0.5f;
            }
            else if (ImGui::GetIO().KeyShift)
                m_app.scroll_x -= wh * 50;
            else
                m_app.scroll_y -= wh * 50;
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
        {
            m_app.dragging = true;
            m_app.drag_start = ImGui::GetMousePos();
            m_app.drag_sx = m_app.scroll_x;
            m_app.drag_sy = m_app.scroll_y;
        }
    }
    if (m_app.dragging)
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            ImVec2 m = ImGui::GetMousePos();
            m_app.scroll_x = m_app.drag_sx - (m.x - m_app.drag_start.x);
            m_app.scroll_y = m_app.drag_sy - (m.y - m_app.drag_start.y);
        }
        else
            m_app.dragging = false;
    }

    m_app.scroll_x = std::clamp(m_app.scroll_x, 0.0f, msx);
    m_app.scroll_y = std::clamp(m_app.scroll_y, 0.0f, msy);

    ImVec2 wp = ImGui::GetCursorScreenPos();
    float ox = sx ? -m_app.scroll_x : (cw - iw) * 0.5f;
    float oy = sy ? -m_app.scroll_y : (ch - ih) * 0.5f;
    ImVec2 p0(wp.x + ox, wp.y + oy);
    ImVec2 p1(p0.x + iw, p0.y + ih);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->PushClipRect(wp, ImVec2(wp.x + cw, wp.y + ch), true);

    if (m_app.show_checker)
    {
        float vx0 = std::max(p0.x, wp.x), vy0 = std::max(p0.y, wp.y);
        float vx1 = std::min(p1.x, wp.x + cw), vy1 = std::min(p1.y, wp.y + ch);
        if (vx0 < vx1 && vy0 < vy1)
            DrawChecker(dl, ImVec2(vx0, vy0), ImVec2(vx1 - vx0, vy1 - vy0));
    }

    if (frame->gl_texture)
        dl->AddImage((ImTextureID)(intptr_t)frame->gl_texture, p0, p1);

    dl->AddRect(p0, p1, IM_COL32(120, 140, 200, 60), 0, 0, 1.0f);
    dl->PopClipRect();

    auto DrawScrollbar = [&](bool horizontal)
    {
        float bar_x, bar_y, bar_w, bar_h;
        float content_sz, img_sz, scroll_val, max_scroll;

        if (horizontal)
        {
            bar_x = wp.x; bar_y = wp.y + ch;
            bar_w = cw; bar_h = sbt;
            content_sz = cw; img_sz = iw;
            scroll_val = m_app.scroll_x; max_scroll = msx;
        }
        else
        {
            bar_x = wp.x + cw; bar_y = wp.y;
            bar_w = sbt; bar_h = ch;
            content_sz = ch; img_sz = ih;
            scroll_val = m_app.scroll_y; max_scroll = msy;
        }

        dl->AddRectFilled(ImVec2(bar_x, bar_y), ImVec2(bar_x + bar_w, bar_y + bar_h), IM_COL32(20, 20, 25, 200), 2.0f);

        float vis = content_sz / img_sz;
        float track = horizontal ? bar_w : bar_h;
        float thumb = std::max(20.0f, track * vis);
        float ratio = (max_scroll > 0) ? (scroll_val / max_scroll) : 0;
        float thumb_pos = ratio * (track - thumb);

        ImVec2 tmin, tmax;
        if (horizontal)
        {
            tmin = ImVec2(bar_x + thumb_pos, bar_y + 2);
            tmax = ImVec2(bar_x + thumb_pos + thumb, bar_y + bar_h - 2);
        }
        else
        {
            tmin = ImVec2(bar_x + 2, bar_y + thumb_pos);
            tmax = ImVec2(bar_x + bar_w - 2, bar_y + thumb_pos + thumb);
        }

        bool hov = ImGui::IsMouseHoveringRect(tmin, tmax);
        ImU32 tc = hov ? IM_COL32(140, 160, 220, 200) : IM_COL32(80, 80, 100, 180);
        dl->AddRectFilled(tmin, tmax, tc, 3.0f);

        ImVec2 bmin(bar_x, bar_y), bmax(bar_x + bar_w, bar_y + bar_h);
        if (ImGui::IsMouseHoveringRect(bmin, bmax) && ImGui::IsMouseClicked(0))
        {
            float click = horizontal ? (ImGui::GetMousePos().x - bar_x) : (ImGui::GetMousePos().y - bar_y);
            float& sv = horizontal ? m_app.scroll_x : m_app.scroll_y;
            sv = (click / track) * max_scroll;
        }
        if (hov && ImGui::IsMouseDragging(0))
        {
            float delta = horizontal ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y;
            float& sv = horizontal ? m_app.scroll_x : m_app.scroll_y;
            sv += delta * (max_scroll / (track - thumb));
        }
    };

    if (sx) 
        DrawScrollbar(true);
    
    if (sy) 
        DrawScrollbar(false);

    if (sx && sy)
        dl->AddRectFilled( ImVec2(wp.x + cw, wp.y + ch), ImVec2(wp.x + cw + sbt, wp.y + ch + sbt), IM_COL32(20, 20, 25, 200), 2.0f);

    ImGui::End();
}

void UI::DrawProperties()
{
    if (!m_app.show_info) 
        return;

    float tb_h = m_app.show_toolbar ? 38.0f : 0.0f;
    float sb_h = 26.0f;
    float pw = 300.0f;

    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x - pw, vp->WorkPos.y + tb_h));
    ImGui::SetNextWindowSize(ImVec2(pw, vp->WorkSize.y - tb_h - sb_h));

    ImGui::Begin("Properties", &m_app.show_info,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (!m_app.sprite_loaded)
    {
        ImGui::TextDisabled("No file loaded");
        ImGui::End();
        return;
    }

    const SpriteData& d = m_app.loader.GetData();

    Section("File");
    {
        std::string fn = GetFilename(d.filepath);
        ImGui::TextUnformatted(fn.c_str());
        Tooltip(d.filepath.c_str());
    }

    Section("Sprite");
    {
        const char* vn = (d.version == 1) ? "Quake" : (d.version == 2) ? "Half-Life" : "Unknown";
        PropRow("Version", "%d (%s)", d.version, vn);
        PropRow("Type", "%s", SpriteLoader::GetTypeString(d.type));
        PropRow("Render", "%s", SpriteLoader::GetTexFormatString(d.texFormat));
        PropRow("Cull", "%s", SpriteLoader::GetFaceTypeString(d.facetype));
        PropRow("Bounds", "%d x %d", d.bounds[0], d.bounds[1]);
        PropRow("Frames", "%d", m_app.total_frames);
    }

    Section("Frame");
    SpriteFrame* fr = m_app.loader.GetFrame(m_app.current_frame);
    if (fr)
    {
        PropRow("Index", "%d / %d", m_app.current_frame + 1, m_app.total_frames);
        PropRow("Size", "%d x %d", fr->width, fr->height);
        PropRow("Origin", "%d, %d", fr->origin[0], fr->origin[1]);
        PropRow("Interval", "%.4f s", fr->interval);

        if (m_app.animating)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.85f, 0.45f, 1.0f));
            ImGui::Text(" Playing");
            ImGui::PopStyleColor();
        }
    }

    Section("Groups");
    int gi = 0;
    for (const auto& g : d.groups)
    {
        const char* ts = (g.type == FRAME_SINGLE) ? "Single" : (g.type == FRAME_GROUP)  ? "Group" : "Angled";
        char lbl[64];
        snprintf(lbl, sizeof(lbl), "%s #%d  (%d)###g%d", ts, gi, (int)g.frames.size(), gi);

        if (ImGui::TreeNode(lbl))
        {
            for (int fi = 0; fi < (int)g.frames.size(); fi++)
            {
                const auto& f = g.frames[fi];
                ImGui::BulletText("%dx%d  (%d,%d)  %.3fs",
                    f.width, f.height, f.origin[0], f.origin[1], f.interval);
            }
            ImGui::TreePop();
        }
        gi++;
    }

    if (d.version == SPRITE_VERSION_HL && d.palette_colors > 0)
    {
        Section("Palette");

        ImVec2 cs(9, 9);
        int cols = 16;
        ImDrawList* pdl = ImGui::GetWindowDrawList();
        ImVec2 base = ImGui::GetCursorScreenPos();

        for (int i = 0; i < d.palette_colors && i < 256; i++)
        {
            int row = i / cols, col = i % cols;
            ImVec2 a(base.x + col * (cs.x + 1), base.y + row * (cs.y + 1));
            ImVec2 b(a.x + cs.x, a.y + cs.y);

            uint8_t r  = d.palette[i * 3];
            uint8_t g  = d.palette[i * 3 + 1];
            uint8_t bl = d.palette[i * 3 + 2];
            pdl->AddRectFilled(a, b, IM_COL32(r, g, bl, 255));

            if (ImGui::IsMouseHoveringRect(a, b))
            {
                pdl->AddRect(a, b, IM_COL32(255, 255, 100, 255), 0, 0, 1.5f);
                ImGui::BeginTooltip();
                ImGui::ColorButton("##c",ImVec4(r / 255.f, g / 255.f, bl / 255.f, 1.f), ImGuiColorEditFlags_NoTooltip, ImVec2(28, 28));
                ImGui::SameLine();
                ImGui::Text("#%d\n%d %d %d\n#%02X%02X%02X", i, r, g, bl, r, g, bl);
                ImGui::EndTooltip();
            }
        }

        int rows = (std::min((int)d.palette_colors, 256) + cols - 1) / cols;
        ImGui::Dummy(ImVec2(cols * (cs.x + 1), rows * (cs.y + 1)));
    }

    ImGui::End();
}

void UI::DrawStatusBar()
{
    float h = 26.0f;
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y + vp->WorkSize.y - h));
    ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 3));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.12f, 1.0f));

    ImGui::Begin("##sb", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (!m_app.status_msg.empty())
    {
        auto now = std::chrono::high_resolution_clock::now();
        double age = std::chrono::duration<double>(
            now.time_since_epoch()).count() - m_app.status_time;
        if (age < 4.0)
        {
            float a = (age < 3.0) ? 1.0f : (float)(4.0 - age);
            ImGui::PushStyleColor(ImGuiCol_Text,
                ImVec4(0.75f, 0.80f, 0.92f, a));
            ImGui::TextUnformatted(m_app.status_msg.c_str());
            ImGui::PopStyleColor();
        }
        else
            m_app.status_msg.clear();
    }

    if (m_app.sprite_loaded)
    {
        const SpriteData& d = m_app.loader.GetData();
        char buf[96];
        snprintf(buf, sizeof(buf), "v%d  %s  %d frames", d.version, SpriteLoader::GetTexFormatString(d.texFormat), m_app.total_frames);
        float tw = ImGui::CalcTextSize(buf).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - tw - 12);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.42f, 0.42f, 0.50f, 1.0f));
        ImGui::TextUnformatted(buf);
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void UI::DrawAbout()
{
    if (!m_app.show_about)
        return;

    ImGui::OpenPopup("About##dlg");
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("About##dlg", &m_app.show_about,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
    {
        float footer_height = ImGui::GetFrameHeightWithSpacing() + 10.0f;

        ImGui::BeginChild("about_content", ImVec2(0, -footer_height), false);

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.62f, 0.95f, 1.0f));
        ImGui::Text("Sprite-Tools");
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::TextWrapped("Sprite viewer and creator for Quake / Half-Life sprites");

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text("Github:");
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(0.45f, 0.62f, 0.95f, 1.0f), "https://github.com/Elinsrc/Sprite-Tools");

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::SetTooltip("Open GitHub repository");

            if (ImGui::IsItemClicked())
            {
#ifdef _WIN32
                ShellExecuteA(0, "open", "https://github.com/Elinsrc/Sprite-Tools", 0, 0, SW_SHOWNORMAL);
#else
                system("xdg-open https://github.com/Elinsrc/Sprite-Tools");
#endif
            }
        }

        ImGui::EndChild();

        ImGui::Separator();

        float button_width = 100.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - button_width) * 0.5f);

        if (ImGui::Button("OK", ImVec2(button_width, 0)))
        {
            m_app.show_about = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
void UI::HandleKeys()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantTextInput || ImGui::IsAnyItemActive())
        return;

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O))
    {
        OpenFileDialog();
        return;
    }

    if (!m_app.sprite_loaded)
        return;

    if (ImGui::IsKeyPressed(ImGuiKey_Space) && m_app.total_frames > 1)
    {
        m_app.animating = !m_app.animating;
        if (m_app.animating)
        {
            auto now = std::chrono::high_resolution_clock::now();
            m_app.last_time = std::chrono::duration<double>(
                now.time_since_epoch()).count();
        }
    }

    if (!m_app.animating && m_app.total_frames > 1)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            m_app.current_frame = (m_app.current_frame - 1 + m_app.total_frames) % m_app.total_frames;

        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            m_app.current_frame = (m_app.current_frame + 1) % m_app.total_frames;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Home))
        m_app.current_frame = 0;

    if (ImGui::IsKeyPressed(ImGuiKey_End))
        m_app.current_frame = std::max(0, m_app.total_frames - 1);

    if (ImGui::IsKeyPressed(ImGuiKey_Equal) ||
        ImGui::IsKeyPressed(ImGuiKey_KeypadAdd))
        m_app.zoom = std::min(16.0f, m_app.zoom * 2.0f);

    if (ImGui::IsKeyPressed(ImGuiKey_Minus) ||
        ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract))
        m_app.zoom = std::max(0.25f, m_app.zoom * 0.5f);

    if (ImGui::IsKeyPressed(ImGuiKey_1))
    {
        m_app.zoom = 1.0f;
        m_app.scroll_x = m_app.scroll_y = 0;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_I))
        m_app.show_info = !m_app.show_info;

    if (ImGui::IsKeyPressed(ImGuiKey_T))
        m_app.show_toolbar = !m_app.show_toolbar;
    
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_E) && m_app.sprite_loaded)
    {
        m_conv.show_export = true;
        return;
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_I))
    {
        m_conv.show_import = true;
        return;
    }
}

void UI::DrawExportDialog()
{
    if (!m_conv.show_export) 
        return;
    
    if (!m_app.sprite_loaded)
    {
        m_conv.show_export = false;
        return;
    }

    if (m_task.running.load()) 
        return;

    ImGui::OpenPopup("Export Frames##dlg");
    ImGui::SetNextWindowSize(ImVec2(360, 200), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Export Frames##dlg", &m_conv.show_export, ImGuiWindowFlags_NoResize))
    {
        Section("Output Format");

        const char* fmts[] = { "PNG (with alpha)", "BMP (no alpha)" };
        ImGui::Combo("Format", &m_conv.export_format, fmts, 2);

        Section("Frame Selection");

        ImGui::RadioButton("All frames", &m_conv.export_frame, -1);
        ImGui::SameLine();
        ImGui::RadioButton("Current only", &m_conv.export_frame, m_app.current_frame);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float bw = 110;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bw * 2 - 8) * 0.5f);

        if (ImGui::Button("Export...", ImVec2(bw, 0)))
        {
            ImGui::CloseCurrentPopup();
            StartExport();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(bw, 0)))
        {
            m_conv.show_export = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UI::DrawImportDialog()
{
    if (!m_conv.show_import) 
        return;
    
    if (m_task.running.load()) 
        return;

    ImGui::OpenPopup("Create SPR##dlg");
    ImGui::SetNextWindowSize(ImVec2(480, 420), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Create SPR##dlg", &m_conv.show_import, ImGuiWindowFlags_NoResize))
    {
        Section("Input Images");

        if (ImGui::Button("Add Images..."))
        {
            auto sel = pfd::open_file("Select Images", m_app.last_dir, { "Images", "*.png *.bmp", "All", "*" }, pfd::opt::multiselect).result();

            for (const auto& s : sel)
                m_conv.import_files.push_back(s);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All"))
            m_conv.import_files.clear();

        ImGui::Text("%d file(s)", (int)m_conv.import_files.size());

        if (!m_conv.import_files.empty())
        {
            ImGui::BeginChild("##filelist", ImVec2(0, 90), ImGuiChildFlags_Borders);

            int rm = -1;
            for (int i = 0; i < (int)m_conv.import_files.size(); i++)
            {
                ImGui::PushID(i);
                if (ImGui::SmallButton("X")) rm = i;
                ImGui::SameLine();
                ImGui::TextUnformatted(GetFilename(m_conv.import_files[i]).c_str());
                ImGui::PopID();
            }
            if (rm >= 0)
                m_conv.import_files.erase(m_conv.import_files.begin() + rm);

            ImGui::EndChild();
        }

        Section("Sprite Parameters");

        const char* vers[] = { "Quake (v1)", "Half-Life (v2)" };
        int vi = m_conv.import_version - 1;
        if (ImGui::Combo("Version", &vi, vers, 2))
            m_conv.import_version = vi + 1;

        const char* types[] = {
            "Parallel Upright", 
            "Facing Upright",
            "Parallel", 
            "Oriented", 
            "Parallel Oriented"
        };
        ImGui::Combo("Type", &m_conv.import_type, types, 5);

        if (m_conv.import_version == 2)
        {
            const char* tfmts[] = {
                "Normal", 
                "Additive",
                "Index Alpha", 
                "Alpha Test"
            };
            ImGui::Combo("Render Mode",&m_conv.import_tex_format, tfmts, 4);
        }

        ImGui::SliderFloat("Interval", &m_conv.import_interval, 0.01f, 1.0f, "%.3f s");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool can = !m_conv.import_files.empty();
        if (!can) 
            ImGui::BeginDisabled();

        float bw = 110;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bw * 2 - 8) * 0.5f);

        if (ImGui::Button("Create SPR...", ImVec2(bw, 0)))
        {
            ImGui::CloseCurrentPopup();
            StartImport();
        }

        if (!can) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(bw, 0)))
        {
            m_conv.show_import = false;
            m_conv.import_files.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UI::StartTask(const std::string& title, std::function<void(TaskState&)> work)
{
    if (m_task.running.load())
        return;

    if (m_task.worker.joinable())
        m_task.worker.join();

    m_task.running.store(true);
    m_task.done.store(false);
    m_task.cancel_requested.store(false);
    m_task.progress.store(0.0f);
    m_task.result_success = false;
    m_task.pending_open_file.clear();

    {
        std::lock_guard<std::mutex> lock(m_task.mutex);
        m_task.title = title;
        m_task.status = "Starting...";
        m_task.result_message.clear();
    }

    m_task.worker = std::thread([this, work]()
    {
        work(m_task);

        m_task.progress.store(1.0f);
        m_task.running.store(false);
        m_task.done.store(true);
    });
}

void UI::DrawProgressDialog()
{
    if (!m_task.running.load() && !m_task.done.load()) 
        return;

    std::string title, status, result;
    {
        std::lock_guard<std::mutex> lock(m_task.mutex);
        title = m_task.title;
        status = m_task.status;
        result = m_task.result_message;
    }

    float progress = m_task.progress.load();
    bool is_running = m_task.running.load();
    bool is_done = m_task.done.load();

    std::string popup_id = title + "##progress";

    ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1), ImVec2(400, -1));

    if (!ImGui::IsPopupOpen(popup_id.c_str()))
        ImGui::OpenPopup(popup_id.c_str());

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings;

    bool open = true;
    if (ImGui::BeginPopupModal(popup_id.c_str(), nullptr, flags))
    {
        if (is_running)
        {
            ImGui::TextWrapped("%s", status.c_str());
            ImGui::Spacing();

            char pct[32];
            snprintf(pct, sizeof(pct), "%.0f%%", progress * 100.0f);
            ImGui::ProgressBar(progress, ImVec2(-1, 0), pct);

            ImGui::Spacing();

            double t = ImGui::GetTime();
            int dots = ((int)(t * 3.0)) % 4;
            char anim[8] = "       ";
            for (int i = 0; i < dots; i++) anim[i] = '.';
            anim[dots] = '\0';

            ImGui::TextDisabled("Working%s", anim);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            float bw = 120.0f;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bw) * 0.5f);
            if (ImGui::Button("Cancel", ImVec2(bw, 0)))
            {
                m_task.cancel_requested.store(true);
                std::lock_guard<std::mutex> lock(m_task.mutex);
                m_task.status = "Cancelling...";
            }
        }
        else if (is_done)
        {
            if (m_task.result_success)
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Success!");
            else
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed!");

            ImGui::Spacing();

            if (!result.empty())
            {
                ImGui::TextWrapped("%s", result.c_str());
                ImGui::Spacing();
            }

            ImGui::ProgressBar(1.0f, ImVec2(-1, 0), "Done");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            float bw = 120.0f;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bw) * 0.5f);
            if (ImGui::Button("OK", ImVec2(bw, 0)))
            {
                m_task.done.store(false);
                ImGui::CloseCurrentPopup();

                if (!m_task.pending_open_file.empty())
                {
                    m_app.pending_file = m_task.pending_open_file;
                    m_task.pending_open_file.clear();
                }

                if (m_task.worker.joinable())
                    m_task.worker.join();
            }
        }

        ImGui::EndPopup();
    }
}

void UI::StartExport()
{
    std::string spr_path = m_app.loader.GetData().filepath;
    int format = m_conv.export_format;
    int frame_idx = m_conv.export_frame;
    int total = m_app.total_frames;

    auto dir = pfd::select_folder("Output Directory", m_app.last_dir).result();

    if (dir.empty()) 
        return;

    std::string prefix = GetFilename(spr_path);
    size_t dot = prefix.rfind('.');
    if (dot != std::string::npos) prefix = prefix.substr(0, dot);

    m_conv.show_export = false;

    StartTask("Exporting Frames", [=](TaskState& task)
    {
        SpriteLoader loader;

        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.status = "Loading sprite...";
        }

        if (!loader.Load(spr_path))
        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = "Failed to load sprite";
            task.result_success = false;
            return;
        }

        int t = loader.GetTotalFrameCount();
        int start = 0, end_idx = t;

        if (frame_idx >= 0 && frame_idx < t)
        {
            start = frame_idx;
            end_idx = frame_idx + 1;
        }

        int count = end_idx - start;
        int exported = 0;
        std::string ext = SpriteConverter::GetFormatExtension(static_cast<ImageFormat>(format));

        for (int i = start; i < end_idx; i++)
        {
            if (task.cancel_requested.load())
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.result_message = "Cancelled. Exported " +
                    std::to_string(exported) + " file(s)";
                task.result_success = false;
                return;
            }

            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.status = "Exporting frame " +
                    std::to_string(i + 1) + " / " +
                    std::to_string(end_idx) + "...";
            }

            SpriteFrame* frame = loader.GetFrame(i);
            if (!frame || frame->rgba.empty())
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.result_message = "Failed to get frame " + std::to_string(i);
                task.result_success = false;
                return;
            }

            char fname[512];
            if (t == 1 && frame_idx < 0)
                snprintf(fname, sizeof(fname), "%s/%s%s", dir.c_str(), prefix.c_str(), ext.c_str());
            else
                snprintf(fname, sizeof(fname), "%s/%s_%03d%s", dir.c_str(), prefix.c_str(), i, ext.c_str());

            SprToImageParams p;
            p.output_dir = dir;
            p.output_prefix = prefix;
            p.format = static_cast<ImageFormat>(format);
            p.frame_index = i;

            if (!SpriteConverter::SaveImageRGBA(fname, frame->rgba.data(), frame->width, frame->height, static_cast<ImageFormat>(format)))
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.result_message = "Failed to save: " + std::string(fname);
                task.result_success = false;
                return;
            }

            exported++;
            task.progress.store((float)(i - start + 1) / (float)count);
        }

        std::lock_guard<std::mutex> lock(task.mutex);
        task.result_message = "Exported " + std::to_string(exported) + " file(s) to:\n" + dir;
        task.result_success = true;
    });
}

void UI::StartImport()
{
    auto save = pfd::save_file("Save Sprite", m_app.last_dir + "/output.spr", { "Sprite (*.spr)", "*.spr" }).result();

    if (save.empty()) return;

    if (save.size() < 4 || save.substr(save.size() - 4) != ".spr")
        save += ".spr";

    std::vector<std::string> files = m_conv.import_files;
    int version = m_conv.import_version;
    int type = m_conv.import_type;
    int tex_format = m_conv.import_tex_format;
    float interval = m_conv.import_interval;

    m_conv.show_import = false;
    m_conv.import_files.clear();

    StartTask("Creating Sprite", [=](TaskState& task)
    {
        int total = (int)files.size();

        std::vector<std::vector<uint8_t>> rgba_storage;
        std::vector<const uint8_t*> rgba_ptrs;
        std::vector<int> widths, heights;

        for (int i = 0; i < total; i++)
        {
            if (task.cancel_requested.load())
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.result_message = "Cancelled";
                task.result_success = false;
                return;
            }

            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.status = "Loading image " +
                    std::to_string(i + 1) + " / " +
                    std::to_string(total) + "...";
            }

            std::vector<uint8_t> rgba;
            int w, h, ch;
            uint8_t* px = stbi_load(files[i].c_str(), &w, &h, &ch, 4);

            if (!px)
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                std::string fn = files[i];
                size_t sl = fn.find_last_of("/\\");
                if (sl != std::string::npos) fn = fn.substr(sl + 1);
                task.result_message = "Failed to load: " + fn;
                task.result_success = false;
                return;
            }

            rgba.assign(px, px + (size_t)w * h * 4);
            stbi_image_free(px);

            if (w > 4096 || h > 4096 || w <= 0 || h <= 0)
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.result_message = "Bad image size: " +
                    std::to_string(w) + "x" + std::to_string(h);
                task.result_success = false;
                return;
            }

            widths.push_back(w);
            heights.push_back(h);
            rgba_storage.push_back(std::move(rgba));

            task.progress.store((float)(i + 1) / (float)(total * 3));
        }

        if (task.cancel_requested.load())
        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = "Cancelled";
            task.result_success = false;
            return;
        }

        for (auto& v : rgba_storage)
            rgba_ptrs.push_back(v.data());

        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.status = "Building palette...";
        }

        task.progress.store(0.4f);

        ImageToSprParams p;
        p.version = version;
        p.type = (uint32_t)type;
        p.tex_format = (uint32_t)tex_format;
        p.interval = interval;

        auto result = SpriteConverter::RGBAToSprMemory(rgba_ptrs, widths, heights, p);

        if (task.cancel_requested.load())
        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = "Cancelled";
            task.result_success = false;
            return;
        }

        task.progress.store(0.8f);

        if (!result.success)
        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = result.error;
            task.result_success = false;
            return;
        }

        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.status = "Saving sprite...";
        }

        std::ofstream file(save, std::ios::binary);
        if (!file.is_open())
        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = "Failed to create file";
            task.result_success = false;
            return;
        }

        file.write(reinterpret_cast<const char*>(result.data.data()),
            (std::streamsize)result.data.size());
        file.close();

        task.progress.store(1.0f);

        std::string fn = save;
        size_t sl = fn.find_last_of("/\\");
        if (sl != std::string::npos) fn = fn.substr(sl + 1);

        {
            std::lock_guard<std::mutex> lock(task.mutex);
            task.result_message = "Created: " + fn + "\n" +
                std::to_string(total) + " frame(s), " +
                std::to_string(result.data.size()) + " bytes";
            task.result_success = true;
            task.pending_open_file = save;
        }
    });
}

void UI::RenderFrame()
{
    HandleKeys();
    ProcessPendingFile();
    DrawMenuBar();
    DrawToolbar();
    DrawViewport();
    DrawProperties();
    DrawStatusBar();
    DrawAbout();
    DrawExportDialog();
    DrawImportDialog();
    DrawProgressDialog();
}