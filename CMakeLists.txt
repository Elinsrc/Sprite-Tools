cmake_minimum_required(VERSION 3.16)
project(SpriteTools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(PLATFORM_WIN32 TRUE)
    message(STATUS "Platform: Windows")
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Platform: Linux")
endif()

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(STB_DIR ${CMAKE_SOURCE_DIR}/external/stb)
set(PFD_DIR ${CMAKE_SOURCE_DIR}/external/portable-file-dialogs)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


find_package(glfw3 3.3 QUIET)

if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

list(APPEND IMGUI_SOURCES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)


set(PROJECT_SOURCES
    ${SRC_DIR}/ui.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/sprite_loader.cpp
    ${SRC_DIR}/renderer.cpp
)

set(ALL_SOURCES
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR} 
    ${PFD_DIR}
)

if(PLATFORM_WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32)
endif()

if(PLATFORM_LINUX)
    find_package(OpenGL REQUIRED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL dl pthread)
endif()
