#include "imgui/imgui.h"
#include <lua_engine/LuaBindings.h>

namespace LuaBindings {
void BindGUIAPIs(sol::state& lua) {
    sol::table g = lua.create_table();
    g["Begin"]            = [](const std::string& n, sol::optional<int> flags) {
                                ImGui::Begin(n.c_str(), nullptr, flags.value_or(0));
                            };
    g["End"]              = []()                              { ImGui::End(); };
    g["Text"]             = [](const std::string& t)         { ImGui::Text("%s", t.c_str()); };
    g["Checkbox"]         = [](const std::string& l, bool v) { ImGui::Checkbox(l.c_str(), &v); return v; };
    g["CollapsingHeader"] = [](const std::string& l) -> bool { return ImGui::CollapsingHeader(l.c_str()); };
    g["Button"]           = [](const std::string& l) -> bool { return ImGui::Button(l.c_str()); };
    g["Separator"]        = []() { ImGui::Separator(); };
    g["Spacing"]          = []() { ImGui::Spacing(); };
    g["SameLine"]         = []() { ImGui::SameLine(); };

    sol::table flags = lua.create_table();
    flags["NoCollapse"]       = static_cast<int>(ImGuiWindowFlags_NoCollapse);
    flags["NoResize"]         = static_cast<int>(ImGuiWindowFlags_NoResize);
    flags["NoMove"]           = static_cast<int>(ImGuiWindowFlags_NoMove);
    flags["NoTitleBar"]       = static_cast<int>(ImGuiWindowFlags_NoTitleBar);
    flags["AlwaysAutoResize"] = static_cast<int>(ImGuiWindowFlags_AlwaysAutoResize);
    g["Flags"] = flags;

    lua["GUI"] = g;
}
} // namespace LuaBindings
