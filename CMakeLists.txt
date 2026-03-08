cmake_minimum_required(VERSION 3.18)
project(SpriteTools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID)
    set(PLATFORM_ANDROID TRUE)
    message(STATUS "Platform: Android")
elseif(WIN32)
    set(PLATFORM_WIN32 TRUE)
    message(STATUS "Platform: Windows")
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Platform: Linux")
endif()

if(PLATFORM_ANDROID)

    add_library(${PROJECT_NAME} SHARED
        src/core/sprite_loader.cpp
        src/core/sprite_core.cpp
        src/core/sprite_jni.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/core)

    target_link_libraries(${PROJECT_NAME} android log)

else()

    add_subdirectory(external/glfw)

    set(SOURCES
        src/main.cpp
        src/ui.cpp
        src/ui_utils.cpp
        src/core/sprite_loader.cpp
        src/renderer.cpp

        external/imgui/imgui.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/core
        src/icons
        external/imgui
        external/imgui/backends
        external/stb
        external/portable-file-dialogs
    )

    if(PLATFORM_WIN32)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32)
    endif()

    if(PLATFORM_LINUX)
        find_package(OpenGL REQUIRED)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)
    endif()

endif()