#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include <vector>
#include "SDL_mixer\include\SDL_mixer.h"
#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class Audio : public Module
{
public:
	Audio(Application* app, bool start_enabled = true);
	~Audio();

	bool Init();
	UPDATE_STATUS Configuration(float dt);
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

private:

	Mix_Music*			music;
	std::vector<Mix_Chunk*>	fx;
	int volume;
};

#endif // __ModuleAudio_H__