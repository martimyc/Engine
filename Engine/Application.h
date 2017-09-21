#ifndef _APPLICATION
#define _APPLICATION

#include <vector>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGUI;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer_3d;
	ModuleCamera3D* camera;
	ModuleGUI* gui;

private:
	Timer	ms_timer;
	float	dt;
	std::vector<Module*> modules;

public:

	Application();
	~Application();

	bool Init();
	UPDATE_STATUS Update();
	bool CleanUp();

	void OpenWebsite(const char* url);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

#endif //_APPLICATION