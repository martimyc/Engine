#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Parson\parson.h"
#include "Globals.h"
#include "Application.h"
#include "Console.h"
#include "Camera3D.h"
#include "Window.h"

Window::Window(const char* name, bool start_enabled) : Module(name, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
Window::~Window()
{}

// Called before render is available
bool Window::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	//Charge config
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* win = json_object_dotget_object(obj, "Window");
	screen_width = json_object_get_number(win, "screen_width");
	screen_height = json_object_get_number(win, "screen_height"); 
	scale = json_object_get_number(win, "scale");
	fullscreen = json_object_get_boolean(win, "fullscreen");
	resizable = json_object_get_boolean(win, "resizable");
	borderless = json_object_get_boolean(win, "borderless");
	fullscreen_desktop = json_object_get_boolean(win, "fullscreen_desktop");

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = screen_width * scale;
		int height = screen_height * scale;
		aspect_ratio = (float)width / height;

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_CONTEXT_MAJOR_VERSION);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_CONTEXT_MINOR_VERSION);

		//---
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		//---

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		//This flag only applies to 3.0 and later versions of opengl https://wiki.libsdl.org/SDL_GLcontextFlag
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

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

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//context
			context = SDL_GL_CreateContext(window);
			if (context == NULL)
			{
				LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
				ret = false;
			}

			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		glewExperimental = GL_TRUE;

		//Glew
		if (GLEW_OK == glewInit())
		{
			LOG("Using Glew %s", glewGetString(GLEW_VERSION));
		}
		else
		{
			GLenum err = glewInit();
			LOG("GLEW initialisation error : %s", glewGetErrorString(err));
			ret = false;
		}
	}

	return ret;
}

UPDATE_STATUS Window::Configuration(float dt)
{
	BROFILER_CATEGORY("Window Configuration", Profiler::Color::Coral)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (App->BeginDockWindow("Window", &config_window))
	{
		JSON_Value* config = json_parse_file("config.json");
		JSON_Object* obj = json_value_get_object(config);
		JSON_Object* win_obj = json_object_dotget_object(obj, "Window");
		JSON_Value* win = json_value_init_object();
		
		static int win_size = 0;
		switch (screen_height)
		{
		case 480:
			win_size = 0;
			break;
		case 600:
			win_size = 1;
			break;
		case 768:
			win_size = 2;
			break;
		case 720:
			win_size = 3;
			break;
		case 1024:
			win_size = 4;
			break;
		case 900:
			win_size = 5;
			break;
		case 1080:
			win_size = 6;
			break;
		default:
			break;
		}
			
		if (ImGui::Combo("Window Size", &win_size, " 720 x 480\0 800 x 600\0 1024 x 768\0 1280 x 720\0 1280 x 1024\0 1600 x 900\0 1920 x 1080\0\0"))
		{
			switch (win_size)
			{
			case 0:
				screen_width = 720;
				screen_height = 480;
				break;
			case 1:
				screen_width = 800;
				screen_height = 600;
				break;
			case 2:
				screen_width = 1024;
				screen_height = 768;
				break;
			case 3:
				screen_width = 1280;
				screen_height = 720;
				break;
			case 4:
				screen_width = 1280;
				screen_height = 1024;
				break;
			case 5:
				screen_width = 1600;
				screen_height = 900;
				break;
			case 6:
				screen_width = 1920;
				screen_height = 1080;
				break;
			default:
				break;
			}
			SDL_SetWindowSize(window, screen_width*scale, screen_height*scale);
			aspect_ratio = (float)screen_width / screen_height;
			App->camera->RecalculateFOV();
			App->SetDockContextSize(screen_width*scale, screen_height*scale);
		}

		json_object_set_number(json_object(win), "screen_width", screen_width);
		json_object_dotset_value(obj, "Window", win);
		json_object_set_number(json_object(win), "screen_height", screen_height);
		json_object_dotset_value(obj, "Window", win);
		
		win_size = scale - 1;
		if (ImGui::Combo("Window Size Multiplier", &win_size, " 1\0 2\0\0"))
		{
			switch (win_size)
			{
			case 0:
				scale = 1;
				break;
			case 1:
				scale = 2;
				break;
			default:
				break;
			}
			SDL_SetWindowSize(window, screen_width*scale, screen_height*scale);
			App->SetDockContextSize(screen_width*scale, screen_height*scale);
		}
		json_object_set_number(json_object(win), "scale", scale);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			if (fullscreen)
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(window, NULL);
		}

		json_object_set_boolean(json_object(win), "fullscreen", fullscreen);
		json_object_dotset_value(obj, "Window", win);

		if (ImGui::Checkbox("Resizable", &resizable))
		{
			if (resizable)
				SDL_SetWindowResizable(window, SDL_TRUE);
			else
				SDL_SetWindowResizable(window, SDL_FALSE);
		}
		json_object_set_boolean(json_object(win), "resizable", resizable);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Borderless", &borderless))
		{
			if (borderless)
				SDL_SetWindowBordered(window, SDL_FALSE);
			else
				SDL_SetWindowBordered(window, SDL_TRUE);
		}
		json_object_set_boolean(json_object(win), "borderless", borderless);
		json_object_dotset_value(obj, "Window", win);
		
		if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
		{
			if(fullscreen_desktop)
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(window, NULL);
		}
		json_object_set_boolean(json_object(win), "fullscreen_desktop", fullscreen_desktop);
		json_object_dotset_value(obj, "Window", win); 


		json_serialize_to_file(config, "config.json");
	}
	App->EndDockWindow();

	return ret;
}

// Called before quitting
bool Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void Window::OpenCloseConfigWindowWindow()
{
	config_window = !config_window;
}

const int Window::GetHeight() const
{
	return screen_height * scale;
}

const int Window::GetWidth() const
{
	return screen_width * scale;
}

const int Window::GetScale() const
{
	return scale;
}

const float Window::GetAspectRatio() const
{
	return aspect_ratio;
}

void Window::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}