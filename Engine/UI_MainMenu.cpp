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
	ImGui::Begin("Menu Window");
	if (ImGui::Button("Test Window"))
	{
		if (!gui->GetActive(UI_TEST))
			gui->Activate(UI_TEST);
		else
			gui->Deactivate(UI_TEST);
	}

	if (ImGui::Button("Random numbers test"))
	{
		if (!gui->GetActive(UI_RAND_TEST))
			gui->Activate(UI_RAND_TEST);
		else
			gui->Deactivate(UI_RAND_TEST);
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
