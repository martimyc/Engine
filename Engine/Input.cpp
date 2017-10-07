#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Globals.h"
#include "Application.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Console.h"
#include "Textures.h"
#include "Input.h"


#define MAX_KEYS 300

Input::Input(const char* name, bool start_enabled) : Module(name, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
UPDATE_STATUS Input::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Input PreUpdate", Profiler::Color::Beige)

	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= App->window->GetScale();
	mouse_y /= App->window->GetScale();
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSdlGL2_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
				mouse_z = e.wheel.y;
				break;

			case SDL_MOUSEMOTION:
				mouse_x = e.motion.x / App->window->GetScale();
				mouse_y = e.motion.y / App->window->GetScale();

				mouse_x_motion = e.motion.xrel / App->window->GetScale();
				mouse_y_motion = e.motion.yrel / App->window->GetScale();
				break;

			case SDL_QUIT:
				quit = true;
				break;

			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer_3d->OnResize(e.window.data1, e.window.data2);
				break;

			case SDL_DROPFILE:
				std::string filename (e.drop.file);
				std::string extension;
				// find the last occurrence of '.'
				size_t pos = filename.find_last_of(".");
				// make sure the poisition is valid
				if (pos != filename.length())
					extension = filename.substr(pos + 1);
				else
					LOG("Coud not find . in the dropef file path");

				if (extension == "dds")
					App->textures->LoadTexture(e.drop.file);
				break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}