#include "StatsLayer.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include "utils/Logger.h"

StatsLayer::StatsLayer(Application& app) : app(app) {}

void StatsLayer::OnInit() {
    gpuName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

bool StatsLayer::OnUpdate(float dt) {
    double now = glfwGetTime();
    if (lastFPSTime == 0.0) lastFPSTime = now;
    fpsFrameCount++;
    if (now - lastFPSTime >= 1.0) {
        mspf = 1000.0f / static_cast<float>(fpsFrameCount);
        fpsFrameCount = 0;
        lastFPSTime += 1.0;
        LOG_WRITE();
    }
    return false;
}

void StatsLayer::OnGUI() {
    ImGui::Begin("##perf", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", gpuName);
    ImGui::Text("%.2f ms (%.0f FPS)", mspf, 1000.0f / mspf);
    ImGui::End();

    app.GetGUI().RenderLog("Log Output", LOG_CONTENTS(), LOG_LINE_LEVELS());
}
