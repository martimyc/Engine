#include "imgui-master\imgui.h"
#include "SDL\include\SDL.h"
#include "Application.h"
#include "UI_MainMenu.h"
#include "ModuleGUI.h"
#include "MathGeoLib-master\src\Geometry\Capsule.h"

UI_MainMenu::UI_MainMenu(ModuleGUI * gui, bool active): UI_Element(UI_MAINMENU, active), gui(gui)
{}

UI_MainMenu::~UI_MainMenu()
{}

bool UI_MainMenu::Update()
{
	ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_Once);

	ImGui::Begin("Menu Window");
	if (ImGui::Button("Test Window"))
		gui->SwitchActivation(UI_TEST);
		

	if (ImGui::Button("Random numbers test"))
	{
		gui->SwitchActivation(UI_RAND_TEST);
			//SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	}

	if (ImGui::Button("Geometry collision test"))
	{
		gui->SwitchActivation(UI_GEOMETRY_COLLISION_TEST);
		//SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	}

	if (ImGui::Button("Quit"))
	{
		SDL_Event e;
		e.type = SDL_QUIT;
		SDL_PushEvent(&e);
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	return true;
}
