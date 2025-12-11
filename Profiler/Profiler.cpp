#include "lib/ImGUI/imgui.h"
#include "lib/ImGUI/imgui_impl_glfw.h"
#include "lib/ImGUI/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 
#include <iostream>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
static const int PROFILER_PORT = 5001;
static const char* SERVER_IP = "127.0.0.1";

#include "include/ProfilerClient.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "LKZ Engine Profiler", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ProfilerClient profiler;
    profiler.Connect(SERVER_IP, PROFILER_PORT);

    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        profiler.Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Engine Metrics");

            if (profiler.IsConnected())
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "[CONNECTED] %s:%d", SERVER_IP, PROFILER_PORT);
            else
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "[DISCONNECTED] Waiting for server...");

            ImGui::Separator();

            const auto& stats = profiler.GetStats();

            float timeSincePacket = profiler.GetTimeSinceLastPacket();
            bool isReceiving = timeSincePacket < 0.15f;

            ImVec4 ledColor = isReceiving ? ImVec4(0, 1, 0, 1) : ImVec4(0.3f, 0.3f, 0.3f, 1);

            ImGui::Text("Network Traffic:");
            ImGui::SameLine();

            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 7, p.y + 7), 5.0f, ImGui::GetColorU32(ledColor));
            ImGui::Dummy(ImVec2(15, 15)); 

            ImGui::SameLine();
            ImGui::Text("%llu Packets received", stats.totalPackets);

            ImGui::Separator();

            ImGui::Text("Server DT:   %.4f ms", stats.currentDeltaTime * 1000.0f);
            ImGui::Text("Server FPS:  %.1f", stats.currentDeltaTime > 0 ? 1.0f / stats.currentDeltaTime : 0.0f);

            ImGui::Spacing();

            if (!stats.dtHistory.empty())
            {
                ImGui::PlotLines("Frame Time",
                    stats.dtHistory.data(),
                    (int)stats.dtHistory.size(),
                    0,
                    "ms",
                    0.0f,
                    0.033f,
                    ImVec2(0, 100));
            }

            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}