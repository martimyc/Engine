#include "Globals.h"
#include "Parson\parson.h"
#include "imgui\imgui.h"
#include "Application.h"
#include "ModuleConsole.h"
#include "ModuleAudio.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, "Audio", start_enabled), music(NULL)
{}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{
	App->LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		App->LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		App->LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		App->LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = true;
	}

	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* audio = json_object_dotget_object(obj, "Audio"); 
	volume = json_object_get_number(audio, "volume");

	return ret;
}

UPDATE_STATUS ModuleAudio::Configuration(float dt)
{
	BROFILER_CATEGORY("Audio Configuration", Profiler::Color::Crimson);
		
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Audio"))
	{
		JSON_Value* config = json_parse_file("config.json");
		JSON_Object* obj = json_value_get_object(config);
		JSON_Object* audio_obj = json_object_dotget_object(obj, "Audio");
		JSON_Value* audio = json_value_init_object();

		if (ImGui::SliderInt("Volume", &volume, 0, 100))
		{
			json_object_set_number(json_object(audio), "volume", volume);
			json_object_dotset_value(obj, "Audio", audio);
			json_serialize_to_file(config, "config.json");
		}
	}
	return ret;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	App->LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
		Mix_FreeMusic(music);

	for (std::vector<Mix_Chunk*>::const_iterator it = fx.begin(); it != fx.end(); ++it)
		Mix_FreeChunk(*it);

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;
	
	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		App->LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				App->LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				App->LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	App->LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		App->LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	Mix_Chunk* chunk = nullptr;
	chunk = fx[id - 1];

	if(chunk != nullptr)
	{
		Mix_PlayChannel(-1, chunk, repeat);
		ret = true;
	}

	return ret;
}