#include "UI_Test.h"
#include "imgui-master\imgui.h"

UI_Test::UI_Test(bool active): UI_Element(UI_TEST, active)
{}

UI_Test::~UI_Test()
{}

bool UI_Test::Update()
{
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::ShowTestWindow(&active);

	return true;
}
