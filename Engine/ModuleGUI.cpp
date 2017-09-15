#include "imgui-master/imgui.h"
#include "SDL\include\SDL.h"
#include "imgui-master\imgui_impl_sdl.h"
#include "glut\glut.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"

ModuleGUI::ModuleGUI(Application * app, bool start_enabled): Module(app,start_enabled)
{}

ModuleGUI::~ModuleGUI()
{}

bool ModuleGUI::Init()
{
	ImGui_ImplSdlGL2_Init(App->window->window);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	return true;
}

UPDATE_STATUS ModuleGUI::Update(float dt)
{
	ImGui_ImplSdlGL2_NewFrame(App->window->window);

	// ui updates loop

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_menu_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_menu_window)
	{
		ImGui::Begin("Menu Window", &show_menu_window);
		ImGui::Text("Hello from another window!");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}


	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp()
{
	ImGui_ImplSdlGL2_Shutdown();
	return true;
}

void ModuleGUI::PreRender()
{
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void ModuleGUI::SetMouseWeel(WEEL_MOVEMENT movement)
{
	g_mouse_weel = movement;
}



