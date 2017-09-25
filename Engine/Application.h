#ifndef _APPLICATION
#define _APPLICATION

#include <vector>
#include <string>
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
	std::string title;
	std::string organization;
	Timer	ms_timer;
	float	dt;
	std::vector<Module*> modules;
	int fps_cap = 0;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	char buf1[128] = "Engine 3D - Marc Latorre && Mart� Maj�";
	char buf2[128] = "UPC CITM";

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