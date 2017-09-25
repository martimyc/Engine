#ifndef _APPLICATION
#define _APPLICATION

#include <vector>
#include <string>
#include "MathGeoLib\src\Time\Clock.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGUI;
class ModuleConsole;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer_3d;
	ModuleCamera3D* camera;
	ModuleGUI* gui;
	ModuleConsole* console;

private:
	std::string title;
	Clock	ms_timer;
	unsigned long last_frame_time = 0;
	unsigned int total_frames;
	float	dt;
	std::vector<Module*> modules;
	int fps_cap = 0;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

public:

	Application();
	~Application();

	bool Init();
	UPDATE_STATUS Update();
	bool CleanUp();

	void OpenWebsite(const char* url);

	const std::string GetTitle() const;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	UPDATE_STATUS CreateConfigMenu();
	UPDATE_STATUS EndConfigMenu();
};

#endif //_APPLICATION