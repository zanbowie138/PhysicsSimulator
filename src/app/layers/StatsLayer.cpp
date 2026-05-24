#include "StatsLayer.h"
#include "Application.h"

#include "utils/Logger.h"

StatsLayer::StatsLayer(Application& app) : app(app) {}

void StatsLayer::OnGUI() {
    ImGui::Begin("##perf", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", app.GetGPUName());
    ImGui::Text("%.2f ms (%.0f FPS)", app.GetMSPF(), app.GetFPS());
    ImGui::End();

    app.GetGUI().RenderLog("Log Output", LOG_CONTENTS(), LOG_LINE_LEVELS());
}
