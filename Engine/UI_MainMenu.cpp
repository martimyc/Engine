#include "imgui-master\imgui.h"
#include "SDL\include\SDL.h"
#include "Application.h"
#include "UI_MainMenu.h"
#include "ModuleGUI.h"

UI_MainMenu::UI_MainMenu(ModuleGUI * gui, bool active): UI_Element(UI_MAINMENU, active), gui(gui)
{}

UI_MainMenu::~UI_MainMenu()
{}

bool UI_MainMenu::Update()
{
	ImGui::SetNextWindowSize(ImVec2(350, 100));
	ImGui::Begin("Menu Window", &active);
	if (ImGui::Button("Test Window"))
	{
		if (!gui->GetActive(UI_TEST))
			gui->Activate(UI_TEST);
		else
			gui->Deactivate(UI_TEST);
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
