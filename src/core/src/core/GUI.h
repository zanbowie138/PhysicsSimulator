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
	void RenderLogInline(const char* key, const std::string& log, const std::vector<Utils::LogLevel>& lineLogLevels);
	void ShowErrorOverlay(const std::string& errorMsg, bool& showError);

	static void Demo() { ImGui::ShowDemoWindow(); }

	static void Render();

	static void Clean();

private:
	static void ApplyStyle();
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
	ApplyStyle();
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

inline void GUI::RenderLogInline(const char* key, const std::string& log, const std::vector<Utils::LogLevel>& lineLogLevels)
{
	auto& [charSkip, lineSkip] = logSkips[key];
	std::string line;
	std::stringstream ss(log.substr(charSkip));
	unsigned long lineIdx = 0;
	bool clearLog = ImGui::Button("Clear Log");

	ImGui::BeginChild(key, ImVec2(0, 0), true);
	ImGui::Indent(8.0f);
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
	ImGui::Unindent(8.0f);
	ImGui::EndChild();

	if (clearLog) {
		charSkip = log.size();
		lineSkip += lineIdx;
	}
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

inline void GUI::ApplyStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;

	// Geometry
	style.WindowRounding    = 6.0f;
	style.FrameRounding     = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding      = 4.0f;
	style.WindowPadding     = ImVec2(12.0f, 12.0f);
	style.FramePadding      = ImVec2(8.0f, 4.0f);
	style.ItemSpacing       = ImVec2(8.0f, 6.0f);

	// Color overrides (on top of dark theme)
	// Heavily based on https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9
	style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
    // style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    // style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    // style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    // style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    // style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    // style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    // style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    // style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	for (auto & col : style.Colors)
	{
		float H, S, V;
		ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

		if( S < 0.1f )
		{
			V = 1.0f - V;
		}
		ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
		if( col.w < 1.00f )
		{
			col.w *= 0.85;
		}
	}
}
