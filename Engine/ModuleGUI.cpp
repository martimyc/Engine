#include "imgui-master/imgui.h"
#include "SDL\include\SDL.h"
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
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = SCREEN_WIDTH * SCREEN_SIZE;
	io.DisplaySize.y = SCREEN_HEIGHT * SCREEN_SIZE;
	io.RenderDrawListsFn = NULL;

	io.Fonts->AddFontDefault();

	SetImGUIKeyCodes(App->window->window);

	return true;
}

UPDATE_STATUS ModuleGUI::Update(float dt)
{
	UpdateGUIInputs(dt);

	ImGui::NewFrame();

	// ui updates loop

	ImGui::Text("Hello, world!");

	ImGui::Render();

	return UPDATE_CONTINUE;
}

void ModuleGUI::UpdateGUIInputs(float dt)
{
	if(font == nullptr)
		CreateDeviceObjects(App->window->window);

	ImGuiIO& io = ImGui::GetIO();

	io.MousePos.x = App->input->GetMouseX();
	io.MousePos.y = App->input->GetMouseY();
	io.MouseDown[0] = App->input->GetMouseButton(0);
	io.MouseDown[1] = App->input->GetMouseButton(1);
	io.MouseDown[2] = App->input->GetMouseButton(2);
	io.MouseDown[3] = App->input->GetMouseButton(3);
	io.MouseDown[4] = App->input->GetMouseButton(4);

	io.DeltaTime = dt > 0.0 ? dt : (float)(1.0f / 60.0f);

	if (g_mouse_weel != WM_NO_MOVEMENT)
	{
		io.MouseWheel = (int)g_mouse_weel;
		g_mouse_weel = WM_NO_MOVEMENT;
	}
}

void ModuleGUI::SetMouseWeel(WEEL_MOVEMENT movement)
{
	g_mouse_weel = movement;
}

bool ModuleGUI::SetImGUIKeyCodes(SDL_Window* window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;

	/*io.RenderDrawListsFn = NULL;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.SetClipboardTextFn = ImGui_ImplSdl_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplSdl_GetClipboardText;
	io.ClipboardUserData = NULL;*/

	return true;
}

bool ModuleGUI::CreateDeviceObjects(SDL_Window* window)
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	// Upload texture to graphics system
	
	/*GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);*/

	// Store our identifier
	//io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

	// Restore state
	//glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}
