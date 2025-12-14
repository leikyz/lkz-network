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
#include "include/Core/Manager/ProfilerState.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // --- 1. INITIALISATION GLFW/OPENGL ---
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "LKZ Engine Profiler", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Activer la VSync

    // --- 2. INITIALISATION IMGUI ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // --- 3. INITIALISATION PROFILER ---
    ProfilerClient profiler;
    profiler.Connect(SERVER_IP, PROFILER_PORT);

    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // --- 4. VARIABLES POUR LES GRAPHIQUES (MÉMOIRE) ---
    const int HISTORY_SIZE = 240; // 4 secondes d'historique à 60fps
    static float fps_history[HISTORY_SIZE] = { 0 };
    static float dt_history[HISTORY_SIZE] = { 0 }; // En ms
    static int history_offset = 0;

    // --- BOUCLE PRINCIPALE ---
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        profiler.Update();

        // Mise à jour des données graphiques (buffer circulaire)
        if (profiler.IsConnected())
        {
            fps_history[history_offset] = ProfilerState::Instance().serverFps;
            // Conversion en millisecondes pour une meilleure lecture (ex: 16.6ms)
            dt_history[history_offset] = ProfilerState::Instance().serverDeltaTime * 1000.0f;

            history_offset = (history_offset + 1) % HISTORY_SIZE;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Récupération de l'espace de travail disponible
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;

        // Configuration du Layout
        float top_height = work_size.y * 0.35f;  // 35% pour le haut
        float bottom_height = work_size.y - top_height - 10; // Le reste pour le bas

        // =========================================================
        // FENÊTRE 1 : SERVER METRICS (Haut Gauche)
        // =========================================================
        {
            ImGui::SetNextWindowPos(work_pos, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(work_size.x * 0.5f, top_height), ImGuiCond_FirstUseEver);

            ImGui::Begin("Server Metrics", NULL, ImGuiWindowFlags_NoCollapse);

            // --- Status ---
            if (profiler.IsConnected())
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "[CONNECTED] %s:%d", SERVER_IP, PROFILER_PORT);
            else
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "[DISCONNECTED] Waiting...");

            ImGui::Separator();

            // --- Performance Text ---
            ImGui::Text("Live Values:");
            ImGui::BulletText("FPS: %.1f", ProfilerState::Instance().serverFps);
            ImGui::BulletText("DT:  %.4f ms", ProfilerState::Instance().serverDeltaTime * 1000.0f);

            ImGui::Separator();

            // --- Network Health (LED) ---
            const auto& stats = profiler.GetStats();
            float timeSincePacket = profiler.GetTimeSinceLastPacket();
            bool isReceiving = timeSincePacket < 0.15f; // Seuil de 150ms
            ImVec4 ledColor = isReceiving ? ImVec4(0, 1, 0, 1) : ImVec4(0.3f, 0.3f, 0.3f, 1);

            ImGui::Text("Network Heartbeat:");
            ImGui::SameLine();

            // Dessin de la LED
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 7, p.y + 7), 5.0f, ImGui::GetColorU32(ledColor));
            ImGui::Dummy(ImVec2(15, 15)); // Espace réservé

            ImGui::SameLine();
            if (isReceiving)
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Active (Last: %.0f ms ago)", timeSincePacket * 1000.0f);
            else
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Lagging / No Signal (%.1f s)", timeSincePacket);

            ImGui::TextDisabled("Total Packets: %llu", stats.totalPackets);

            ImGui::End();
        }

        // =========================================================
        // FENÊTRE 2 : GAME DATA (Haut Droite)
        // =========================================================
        {
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + (work_size.x * 0.5f), work_pos.y), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(work_size.x * 0.5f, top_height), ImGuiCond_FirstUseEver);

            ImGui::Begin("Game Data", NULL, ImGuiWindowFlags_NoCollapse);

            int clientCount = ProfilerState::Instance().connectedClientsCount;

            ImGui::Text("Connected Clients:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "%d", clientCount);

            ImGui::Separator();

            if (clientCount > 0)
            {
                ImGui::TextWrapped("List of clients would appear here...");
            }
            else
            {
                ImGui::TextDisabled("No clients connected.");
            }

            ImGui::End();
        }

        // =========================================================
        // FENÊTRE 3 : PERFORMANCE GRAPHS (Bas)
        // =========================================================
        {
            ImGui::SetNextWindowPos(ImVec2(work_pos.x, work_pos.y + top_height), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(work_size.x, bottom_height), ImGuiCond_FirstUseEver);

            ImGui::Begin("Performance History", NULL, ImGuiWindowFlags_NoCollapse);

            // Graphique FPS
            ImGui::Text("FPS History (0-144)");
            ImGui::PlotLines("##FPS", fps_history, HISTORY_SIZE, history_offset,
                NULL, 0.0f, 144.0f, ImVec2(-1, 80));

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            // Graphique Frame Time (DeltaTime en ms)
            // Échelle de 0 à 33ms (Si ça dépasse 33ms, on est sous les 30 FPS)
            ImGui::Text("Frame Time History (ms)");
            ImGui::PlotLines("##DT", dt_history, HISTORY_SIZE, history_offset,
                NULL, 0.0f, 33.0f, ImVec2(-1, 80));

            ImGui::End();
        }

        // --- RENDU FINAL ---
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // --- NETTOYAGE ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}