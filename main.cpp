#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

// GUI Libraries (Dear ImGui & GLFW)
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Image Save Library
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// 迷路のセル状態
enum Cell {
    WALL = 0,
    PATH = 1
};

// 迷路生成クラス（穴掘り法）
class MazeGenerator {
public:
    int width;
    int height;
    std::vector<std::vector<int>> grid;

    MazeGenerator(int w, int h) {
        // 偶数サイズは奇数に補正（穴掘り法の制約）
        width = (w % 2 == 0) ? w + 1 : w;
        height = (h % 2 == 0) ? h + 1 : h;
        Reset();
    }

    void Reset() {
        grid.assign(height, std::vector<int>(width, WALL));
    }

    void Generate() {
        Reset();
        // 起点を奇数座標に設定
        std::random_device rd;
        std::mt19937 g(rd());
        
        int startX = 1;
        int startY = 1;
        grid[startY][startX] = PATH;
        
        Dig(startX, startY, g);
    }

    void SaveToPNG(const char* filename, int cellSize = 10) {
        int imgWidth = width * cellSize;
        int imgHeight = height * cellSize;
        std::vector<unsigned char> imgData(imgWidth * imgHeight * 3, 255); // 白色で初期化

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x] == WALL) {
                    // 壁を黒色にする
                    for (int cy = 0; cy < cellSize; ++cy) {
                        for (int cx = 0; cx < cellSize; ++cx) {
                            int px = (x * cellSize + cx);
                            int py = (y * cellSize + cy);
                            int idx = (py * imgWidth + px) * 3;
                            imgData[idx] = 0;     // R
                            imgData[idx + 1] = 0; // G
                            imgData[idx + 2] = 0; // B
                        }
                    }
                }
            }
        }
        stbi_write_png(filename, imgWidth, imgHeight, 3, imgData.data(), imgWidth * 3);
    }

private:
    void Dig(int x, int y, std::mt19937& g) {
        // 4方向の移動ベクトル（2マスずつ進む）
        int dx[] = {0, 2, 0, -2};
        int dy[] = {-2, 0, 2, 0};
        
        std::vector<int> dirs = {0, 1, 2, 3};
        std::shuffle(dirs.begin(), dirs.end(), g);

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[dirs[i]];
            int ny = y + dy[dirs[i]];

            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1) {
                if (grid[ny][nx] == WALL) {
                    // 間の壁と、進んだ先を道にする
                    grid[y + dy[dirs[i]] / 2][x + dx[dirs[i]] / 2] = PATH;
                    grid[ny][nx] = PATH;
                    Dig(nx, ny, g);
                }
            }
        }
    }
};

// GLFWのエラーコールバック
static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // OpenGLのバージョン設定 (3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS対応
#endif

    // ウィンドウ作成
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Multi-OS Maze Generator", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync有効

    // ImGuiセットアップ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // アプリケーション状態
    int mazeWidth = 31;
    int mazeHeight = 31;
    int cellSize = 8;
    char filename[128] = "maze.png";
    std::string statusMessage = "Click 'Generate Maze' to start.";

    MazeGenerator maze(mazeWidth, mazeHeight);

    // メインループ
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ImGuiフレーム開始
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 操作GUIパネル
        ImGui::Begin("Maze Control Panel");
        
        ImGui::SliderInt("Width", &mazeWidth, 11, 99);
        ImGui::SliderInt("Height", &mazeHeight, 11, 99);
        ImGui::SliderInt("PNG Cell Size (px)", &cellSize, 2, 32);
        ImGui::InputText("Output Filename", filename, IM_ARRAYSIZE(filename));

        if (ImGui::Button("Generate Maze", ImVec2(150, 30))) {
            maze = MazeGenerator(mazeWidth, mazeHeight);
            maze.Generate();
            statusMessage = "Maze generated successfully!";
        }

        ImGui::SameLine();
        if (ImGui::Button("Save to PNG", ImVec2(150, 30))) {
            if (!maze.grid.empty() && maze.grid[1][1] == PATH) {
                maze.SaveToPNG(filename, cellSize);
                statusMessage = "Saved to " + std::string(filename);
            } else {
                statusMessage = "Error: Generate a maze first.";
            }
        }

        ImGui::Text("%s", statusMessage.c_str());
        ImGui::End();

        // 迷路のプレビュー描画パネル
        ImGui::Begin("Maze Preview");
        if (!maze.grid.empty()) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            float displayCellSize = 6.0f; // プレビュー用の固定セルサイズ

            for (int y = 0; y < maze.height; ++y) {
                for (int x = 0; x < maze.width; ++x) {
                    ImU32 color = (maze.grid[y][x] == WALL) ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
                    draw_list->AddRectFilled(
                        ImVec2(p.x + x * displayCellSize, p.y + y * displayCellSize),
                        ImVec2(p.x + (x + 1) * displayCellSize, p.y + (y + 1) * displayCellSize),
                        color
                    );
                }
            }
        }
        ImGui::End();

        // レンダリング
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 後処理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
