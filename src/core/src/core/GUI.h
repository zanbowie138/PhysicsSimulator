#pragma once

#include <functional>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Changed
{
	public:
	bool oldVal, changed = false;

	Changed() = default;
	inline void Update(bool newValue)
	{
		changed = oldVal != newValue;
		oldVal = newValue;
	}
};

class GUI
{
public:
	inline GUI(GLFWwindow* window);

	struct configInfo
	{
		bool showDynamicBoxes;
		bool showOnlyDynamicLeaf;
		bool showStaticBoxes;
		bool showOnlyStaticLeaf;
	} mConfigInfo;


	static bool MouseOver();
	static void SetMouse(bool value);


	void Init(GLFWwindow* window);
	static void NewFrame();

	static void StartWindow(const char* windowName);
	static void EndWindow() { ImGui::End(); }

	static void Text(const char* text) { ImGui::Text(text); }
	static void Checkbox(const char* label, bool* variable) { ImGui::Checkbox(label, variable); }
	static void Button(const char* text, std::function<void()> func);

	void ShowConfigWindow();

	static void Demo() { ImGui::ShowDemoWindow(); }

	static void Render();

	static void Clean();
};

inline GUI::GUI(GLFWwindow* window): mConfigInfo()
{
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
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
	bool windowInitialized = ImGui::Begin(windowName);
	assert(windowInitialized && "Gui window did not start corrently...");
}

inline void GUI::Button(const char* text, std::function<void()> func)
{
	if (ImGui::Button(text))
	{
		func();
	}
}

inline void GUI::ShowConfigWindow()
{
	StartWindow("Config");
	if (ImGui::CollapsingHeader("Dynamic BVH Tree"))
	{
		Checkbox("Show Bounding Boxes", &mConfigInfo.showDynamicBoxes);
		Checkbox("Show only leaf nodes", &mConfigInfo.showOnlyDynamicLeaf);
	}
	if (ImGui::CollapsingHeader("Static BVH Tree"))
	{
		Checkbox("Show Bounding Boxes", &mConfigInfo.showStaticBoxes);
		Checkbox("Show only leaf nodes", &mConfigInfo.showOnlyStaticLeaf);
	}
	EndWindow();
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
}
