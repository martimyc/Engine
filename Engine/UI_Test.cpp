#include "UI_Test.h"
#include "imgui\imgui.h"

UI_Test::UI_Test(bool active): UI_Element(UI_TEST, active)
{}

UI_Test::~UI_Test()
{}

bool UI_Test::Update()
{
	ImGui::SetNextWindowPos(ImVec2(715, 15), ImGuiCond_Once);
	ImGui::ShowTestWindow(&active);

	return true;
}
