cmake_minimum_required(VERSION 3.10)
project(MazeGeneratorGUI)

set(CMAKE_CXX_STANDARD 17)

# FetchContent等でGLFW、ImGuiをリンク、もしくはローカル配置したファイルを指定
# 以下はプロジェクト内にソースファイルを同梱した場合の最小構成例
add_executable(MazeGenerator
    main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# 各OSに応じたOpenGLライブラリのリンク
find_package(GLFW REQUIRED)
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    target_link_libraries(MazeGenerator glfw ${OPENGL_LIBRARY} "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(PROJECT_SYSTEM_NAME STREQUAL "Linux")
    find_package(OpenGL REQUIRED)
    target_link_libraries(MazeGenerator glfw OpenGL::GL X11 pthread dl)
else() # Windows
    find_package(OpenGL REQUIRED)
    target_link_libraries(MazeGenerator glfw OpenGL::GL)
endif()
