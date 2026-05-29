cmake_minimum_required(VERSION 3.16)
project(MazeGenerator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW3 の自動探索
find_package(glfw3 3.3 REQUIRED)

# すべて直下にあるので、ソースファイルをすべて指定
set(PROJECT_SOURCES
    main.cpp
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

# 実行ファイルの定義
add_executable(MazeGenerator ${PROJECT_SOURCES})

# インクルードパスの設定（直下のヘッダーファイルを読み込むため）
target_include_directories(MazeGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 外部ライブラリのリンク設定
target_link_libraries(MazeGenerator PRIVATE glfw)

# OSごとのグラフィックスライブラリの個別リンク
if(UNIX AND NOT APPLE)
    # Linux (Ubuntu)
    target_link_libraries(MazeGenerator PRIVATE GL X11 pthread dl)
elseif(APPLE)
    # macOS
    target_link_libraries(MazeGenerator PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
