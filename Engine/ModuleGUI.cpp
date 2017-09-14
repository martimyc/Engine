#include "ModuleGUI.h"
#include "imgui-master/imgui.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "ModuleInput.h"

bool ModuleGUI::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = SCREEN_WIDTH * SCREEN_SIZE;
	io.DisplaySize.y = SCREEN_HEIGHT * SCREEN_SIZE;
	io.RenderDrawListsFn = NULL;

	//unsigned char* pixels;
	//int width, height;
	//io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	io.Fonts->AddFontDefault();

	return true;
}

UPDATE_STATUS ModuleGUI::Update(float dt)
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos.x = App->input->GetMouseX();
	io.MousePos.y = App->input->GetMouseY();
	io.MouseDown[0] = App->input->GetMouseButton(0);
	io.MouseDown[1] = App->input->GetMouseButton(1);
	io.MouseDown[2] = App->input->GetMouseButton(2);
	io.MouseDown[3] = App->input->GetMouseButton(3);
	io.MouseDown[4] = App->input->GetMouseButton(4);
	//io.MouseWheel = App->input->

	// Setup time step
	Uint32	time = SDL_GetTicks();
	double current_time = time / 1000.0;
	io.DeltaTime = g_time > 0.0 ? (float)(current_time - g_time) : (float)(1.0f / 60.0f);
	g_time = current_time;

	ImGui::NewFrame();

	ImGui::Begin("My window");
	ImGui::Text("Hello, world!");

	ImGui::Render();

	return UPDATE_CONTINUE;
}
