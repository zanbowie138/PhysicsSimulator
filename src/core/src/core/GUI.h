#pragma once

#include <functional>

#include <utils/Logger.h>

#include "GlobalTypes.h"

class GUI
{
public:
	inline GUI(GLFWwindow* window);

	// Per-window log skip state (char offset, line offset)
	std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> logSkips;


	static bool MouseOver();
	static void SetMouse(bool value);


	void Init(GLFWwindow* window);
	static void NewFrame();

	static void StartWindow(const char* windowName);
	static void EndWindow() { ImGui::End(); }

	static void Text(const char* text) { ImGui::Text(text); }
	static void Checkbox(const char* label, bool* variable) { ImGui::Checkbox(label, variable); }
	static void ButtonFunc(const char* label, std::function<void()> func);

	void RenderLog(const char* windowName, const std::string& log, const std::vector<Utils::LogLevel>& lineLogLevels);
	void ShowErrorOverlay(const std::string& errorMsg, bool& showError);

	static void Demo() { ImGui::ShowDemoWindow(); }

	static void Render();

	static void Clean();
};

inline GUI::GUI(GLFWwindow* window)
{
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	LOG(LOG_INFO) << "ImGUI context created.\n";

	auto& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	io.FontGlobalScale = 2.0f;
}

inline bool GUI::MouseOver()
{
	return ImGui::GetIO().WantCaptureMouse;
}

inline void GUI::SetMouse(const bool value)
{
	ImGuiIO& io = ImGui::GetIO();

	if (!value)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	}
}

inline void GUI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

inline void GUI::StartWindow(const char* windowName)
{
	ImGui::Begin(windowName);
}

inline void GUI::ButtonFunc(const char* label, std::function<void()> func)
{
	if (ImGui::Button(label))
	{
		func();
	}
}

inline void GUI::RenderLog(const char* windowName, const std::string& log, const std::vector<Utils::LogLevel>& lineLogLevels)
{
	StartWindow(windowName);

	auto& [charSkip, lineSkip] = logSkips[windowName];
	std::string line;
	std::stringstream ss(log.substr(charSkip));
	unsigned long lineIdx = 0;
	bool clearLog = ImGui::Button("Clear Log");

	while (std::getline(ss, line, '\n'))
    {
		auto logLevel = lineLogLevels[lineIdx + lineSkip];

		if (logLevel == LOG_WARNING) { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255)); }
		else if (logLevel == LOG_ERROR) { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); }
		else { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); }

		ImGui::TextWrapped(line.c_str());
		ImGui::PopStyleColor();

		lineIdx++;
    }

	if (clearLog) {
    	charSkip = log.size();
    	lineSkip += lineIdx;
    }

    EndWindow();
}

inline void GUI::ShowErrorOverlay(const std::string& errorMsg, bool& showError)
{
	if (!showError) return;

	// Center window
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
	                        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);

	// Modal-style window
	ImGui::Begin("Scene Load Error", &showError,
	             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
	ImGui::TextWrapped("%s", errorMsg.c_str());
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Press Ctrl+R to reload scene after fixing the script.");

	ImGui::Spacing();

	if (ImGui::Button("Dismiss", ImVec2(120, 0))) {
		showError = false;
	}

	ImGui::End();
}

inline void GUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

inline void GUI::Clean()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	LOG(LOG_INFO) << "ImGUI context destroyed.\n";
}
