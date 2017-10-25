#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Globals.h"
#include "Application.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Console.h"
#include "Texture.h"
#include "TextureImporter.h"
#include "ImportManager.h"
#include "Camera3D.h"
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
				std::string full_path (e.drop.file);
				std::string extension;
				// find the last occurrence of '.'
				size_t pos = full_path.find_last_of(".");
				// make sure the poisition is valid
				if (pos != full_path.length())
					extension = full_path.substr(pos + 1);
				else
					LOG("Coud not find . in the dropef file path");
				
				//DevIL
				if (IsImageFormat(extension))
					App->import_manager->ImportFromPath(full_path, IT_TEXTURE);//LoadTextureStraightFromPath(full_path);
				//Assimp
				else if (extension == "fbx" || extension == "FBX")
				{
					App->import_manager->ImportFromPath(full_path, IT_SCENE);
					//App->camera->CenterToObj();
				}
				else if (extension == ".mm")
				{
					size_t pos = full_path.find_last_of("\\");
					size_t count = full_path.find_last_of(".") - pos;
					std::string file_name(full_path.substr(pos + 1, count - 1));
					//TODO should check path to se if in assets
					//App->import_manager->Load(file_name);
				}
				else
					LOG("Unknown file type");
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

bool Input::IsImageFormat(const std::string& extension) const
{
	if (extension == "bmp" ||												//Windows Bitmap
		extension == "cut" ||												//Dr.Halo
		extension == "dcx" ||												//Multi - PCX
		extension == "dcm" || extension == "dicom" ||						//Dicom
		extension == "dds" ||												//DirectDraw Surface
		extension == "exr" ||												//OpenEXR
		extension == "fits" || extension == "fit" ||							//Flexible Image Transport System
		extension == "ftx" ||												//Heavy Metal : FAKK 2
		extension == "hdr" ||												//Radiance High Dynamic
		extension == "icns" ||												//Macintosh icon
		extension == "ico" || extension == "cur" ||							//Windows icon / cursor
		extension == "iff" ||												//Interchange File Format
		extension == "iwi" ||												//Infinity Ward Image
		extension == "gif" ||												//Graphics Interchange Format
		extension == "jpg" || extension == "jpe" || extension == "jpeg" ||	//Jpeg
		extension == "jp2" ||												//Jpeg 2000
		extension == "lbm" ||												//Interlaced Bitmap
		extension == "lif" ||												//Homeworld texture
		extension == "mdl" ||												//Half - Life Model 
		extension == "mp3" ||												//MPEG - 1 Audio Layer 3
		extension == "pal" ||												//Palette
		extension == "pcd" ||												//Kodak PhotoCD
		extension == "pcx" ||												//ZSoft PCX
		extension == "pic" ||												//Softimage PIC
		extension == "png" ||												//Portable Network Graphics
		extension == "pbm" || extension == "pgm" ||							//Portable Anymap
		extension == "pnm" || extension == "pnm" ||
		extension == "pix" ||												//Alias | Wavefront
		extension == "psd" ||												//Adobe PhotoShop
		extension == "psp" ||												//PaintShop Pro
		extension == "pxr" ||												//Pixar
		extension == "raw" ||												//Raw data
		extension == "rot" ||												//Homeworld 2 Texture
		extension == "sgi" || extension == "bw" ||							//Portable Anymap
		extension == "rgb" || extension == "rgba" ||
		extension == "texture" ||											//Creative Assembly Texture
		extension == "tga" ||												//Truevision Targa
		extension == "tif" ||												//Tagged Image File Format
		extension == "tpl" ||												//Gamecube Texture
		extension == "utx" ||												//Unreal Texture
		extension == "wal" ||												//Quake 2 Texture
		extension == "vtf" ||												//Valve Texture Format
		extension == "wdp" || extension == "hdp" ||							//HD Photo
		extension == "xpm" 													//X Pixel Map 
		)
		return true;
	return false;
}
