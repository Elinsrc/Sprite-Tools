cmake_minimum_required(VERSION 3.18)

if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()

project(SpriteTools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
        src/core/sprite_converter_capi.cpp
        src/core/sprite_converter.cpp
        src/core/sprite_loader.cpp
        src/core/sprite_core.cpp
        src/core/sprite_jni.cpp
        src/core/stb_impl.cpp
    )
    target_include_directories(${PROJECT_NAME} PRIVATE src/core external/stb)
    target_link_libraries(${PROJECT_NAME} android log)

else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    
    if(PLATFORM_WIN32)
        set(QT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/6.10.0/msvc2022_64")
    else()
        set(QT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/6.10.0/gcc_64")
    endif()

    set(CMAKE_PREFIX_PATH ${QT_ROOT})
    
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets NO_DEFAULT_PATH PATHS ${QT_ROOT})
    
    qt_standard_project_setup()

    set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/sprite_viewport.cpp
        src/properties_panel.cpp
        src/export_dialog.cpp
        src/import_dialog.cpp
        src/about_dialog.cpp
        src/frame_slider.cpp
        src/core/stb_impl.cpp
        src/core/sprite_loader.cpp
        src/core/sprite_converter.cpp
        resources/resources.qrc
    )

    set(HEADERS
        src/mainwindow.h
        src/sprite_viewport.h
        src/properties_panel.h
        src/export_dialog.h
        src/import_dialog.h
        src/about_dialog.h
        src/frame_slider.h
        src/app_state.h
        src/theme.h
    )

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    target_include_directories(${PROJECT_NAME} PRIVATE src src/core external/stb)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

    if(PLATFORM_WIN32)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WIN32)
        set(LIB_DEST ".")
    endif()

    if(PLATFORM_LINUX)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
        set(LIB_DEST "lib")
        
        set_target_properties(${PROJECT_NAME} PROPERTIES 
            BUILD_RPATH "${QT_ROOT}/lib"
            INSTALL_RPATH "$ORIGIN/lib"
            BUILD_WITH_INSTALL_RPATH FALSE
        )
    endif()

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION .
        LIBRARY DESTINATION ${LIB_DEST}
    )

    set(QT_DEPLOY_BIN_DIR ".")
    set(QT_DEPLOY_LIB_DIR "${LIB_DEST}")
    set(QT_DEPLOY_PLUGINS_DIR "plugins")
    
    if(IS_ABSOLUTE "${CMAKE_INSTALL_PREFIX}")
        set(QT_DEPLOY_QT_CONF_PATH "${CMAKE_INSTALL_PREFIX}/qt.conf")
    else()
        get_filename_component(ABS_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
        set(QT_DEPLOY_QT_CONF_PATH "${ABS_INSTALL_PREFIX}/qt.conf")
    endif()

    qt_generate_deploy_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_TRANSLATIONS
        POST_EXCLUDE_REGEXES "" 
    )

    install(SCRIPT ${deploy_script})

    if(PLATFORM_LINUX)
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" 
            "[Paths]\nPrefix = .\nLibraries = lib\nPlugins = plugins\n"
        )
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" DESTINATION .)
    endif()

endif()
