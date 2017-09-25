#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Globals.h"
#include "Application.h"
#include "Parson\parson.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, "Window", start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{}

// Called before render is available
bool ModuleWindow::Init()
{
	App->LOG("Init SDL window & surface");
	bool ret = true;

	//Charge config
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* win = json_object_dotget_object(obj, "Window");
	screen_width = json_object_get_number(win, "screen_width");
	screen_height = json_object_get_number(win, "screen_height"); 
	screen_size = json_object_get_number(win, "screen_size");
	fullscreen = json_object_get_boolean(win, "fullscreen");
	resizable = json_object_get_boolean(win, "resizable");
	borderless = json_object_get_boolean(win, "borderless");
	fullscreen_desktop = json_object_get_boolean(win, "fullscreen_desktop");


	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		App->LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = screen_width * screen_size;
		int height = screen_height * screen_size;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		//---
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		//---

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
		SDL_GL_CreateContext(window);
		ImGui_ImplSdlGL2_Init(window);

		if(window == NULL)
		{
			App->LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

UPDATE_STATUS ModuleWindow::Configuration(float dt)
{
	BROFILER_CATEGORY("Window Configuration", Profiler::Color::Coral)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Window"))
	{
		//TODO CHANGE THIS
		JSON_Value* config = json_parse_file("config.json");
		JSON_Object* obj = json_value_get_object(config);
		JSON_Object* win_obj = json_object_dotget_object(obj, "Window");
		JSON_Value* win = json_value_init_object();

		if (ImGui::SliderInt("Screen Width", &screen_width, 512, 1920))
			SDL_SetWindowSize(window, screen_width*screen_size, screen_height*screen_size);
		json_object_set_number(json_object(win), "screen_width", screen_width);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::SliderInt("Screen Height", &screen_height, 256, 1080))
			SDL_SetWindowSize(window, screen_width*screen_size, screen_height*screen_size);
		json_object_set_number(json_object(win), "screen_height", screen_height);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::SliderInt("Screen Size", &screen_size, 1, 2))
			SDL_SetWindowSize(window, screen_width*screen_size, screen_height*screen_size);
		json_object_set_number(json_object(win), "screen_size", screen_size);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			if (!fullscreen)
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				fullscreen = true;
			}
			else
			{
				SDL_SetWindowFullscreen(window, NULL);
				fullscreen = false;
			}
		}
		json_object_set_boolean(json_object(win), "fullscreen", fullscreen);
		json_object_dotset_value(obj, "Window", win);

		if (ImGui::Checkbox("Resizable", &resizable))
		{
			if (!resizable)
			{
				resizable = true;
				//SDL_SetWindowResizable(SDL_WINDOW_RESIZABLE, resizable);
			}
			else
			{
				resizable = false;
				//SDL_SetWindowResizable(SDL_WINDOW_RESIZABLE, resizable);
			}
		}
		json_object_set_boolean(json_object(win), "resizable", resizable);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Borderless", &borderless))
		{
			if (!borderless)
			{
				borderless = true;
				SDL_SetWindowBordered(window, SDL_TRUE);
			}
			else
			{
				borderless = false;
				SDL_SetWindowBordered(window, SDL_FALSE);
			}

		}
		json_object_set_boolean(json_object(win), "borderless", borderless);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
		{
			if(!fullscreen_desktop)
			{
				fullscreen_desktop = true;
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else
			{
				fullscreen_desktop = false;
				SDL_SetWindowFullscreen(window, NULL);
			}
		}
		json_object_set_boolean(json_object(win), "fullscreen_desktop", fullscreen_desktop);
		json_object_dotset_value(obj, "Window", win); 


		json_serialize_to_file(config, "config.json");
	}
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	App->LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_GLContext glcontext = SDL_GL_GetCurrentContext();
		SDL_GL_DeleteContext(glcontext);
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}