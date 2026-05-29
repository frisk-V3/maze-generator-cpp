cmake_minimum_required(VERSION 3.16)
project(MazeGenerator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 外部ライブラリの自動ダウンロード設定 (Dear ImGui)
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com
    GIT_TAG        v1.90.4 # バージョン1.90以降を指定
)
FetchContent_MakeAvailable(imgui)

# 2. GLFW3 の自動探索
find_package(glfw3 3.3 REQUIRED)

# 3. あなたのソースファイル（リポジトリにあるものだけ指定）
set(PROJECT_SOURCES
    main.cpp
)

# ImGuiのソースファイルをビルド対象に追加
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# 実行ファイルの定義
add_executable(MazeGenerator ${PROJECT_SOURCES} ${IMGUI_SOURCES})

# インクルードパスの設定（直下とImGuiのフォルダを指定）
target_include_directories(MazeGenerator PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

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
