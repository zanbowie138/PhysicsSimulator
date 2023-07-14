#pragma once

#include <functional>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GUI
{
public:
	struct configInfo
	{
		bool debugBoundingBoxes;
		bool showOnlyLeafNodes;
		bool showStaticBoxes;
	} mConfigInfo;

	bool MouseOver() const;
	void SetMouse(bool value);


	void Init(GLFWwindow* window);
	void NewFrame();

	void StartWindow(const char* windowName);
	void EndWindow();

	void Text(const char* text);
	void Checkbox(const char* label, bool* variable);
	void Button(const char* text, std::function<void()> func);

	void ShowConfigWindow();

	void Demo();

	void Render();

	void Clean();
};

inline bool GUI::MouseOver() const
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

inline void GUI::Init(GLFWwindow* window)
{
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

inline void GUI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

inline void GUI::StartWindow(const char* windowName)
{
	assert(ImGui::Begin(windowName) && "Gui window did not start corrently...");
}

inline void GUI::EndWindow()
{
	ImGui::End();
}

inline void GUI::Text(const char* text)
{
	ImGui::Text(text);
}

inline void GUI::Checkbox(const char* label, bool* variable)
{
	ImGui::Checkbox(label, variable);
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
		Checkbox("Show Bounding Boxes", &mConfigInfo.debugBoundingBoxes);
		Checkbox("Show only leaf nodes", &mConfigInfo.showOnlyLeafNodes);
	}
	
	EndWindow();
}

inline void GUI::Demo()
{
	ImGui::ShowDemoWindow();
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
