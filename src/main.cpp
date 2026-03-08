#include <cstdio>

#include "ui.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <GLFW/glfw3.h>

static UI* g_ui = nullptr;

static void DropCallback(GLFWwindow*, int count, const char** paths)
{
    if (g_ui && count > 0)
        g_ui->SetPendingFile(paths[0]);
}

int main(int argc, char* argv[])
{
    glfwSetErrorCallback([](int err, const char* desc)
    {
        fprintf(stderr, "GLFW %d: %s\n", err, desc);
    });

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sprite-Tools", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    UI ui;
    g_ui = &ui;
    
    ui.LoadIcons();
    ui.SetupTheme();

    glfwSetDropCallback(window, DropCallback);

    if (argc > 1)
        ui.SetPendingFile(argv[1]);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ui.ProcessPendingFile();

        if (ui.IsTitleChanged())
            glfwSetWindowTitle(window, ui.ConsumeTitle().c_str());

        if (ui.IsExitRequested())
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui.RenderFrame();

        ImGui::Render();
        int dw, dh;
        glfwGetFramebufferSize(window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ui.CleanupResources();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    g_ui = nullptr;

    return 0;
}