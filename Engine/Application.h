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
class ModuleConsole;
class ModuleHardware;
class ModuleMainMenuBar;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer_3d;
	ModuleCamera3D* camera;
	ModuleConsole* console;
	ModuleHardware* hardware;
	ModuleMainMenuBar* main_menu_bar;

private:
	std::string title;
	std::string organization;
	Clock	ms_timer;
	unsigned long last_frame_time = 0;
	unsigned int total_frames;
	float	dt;
	std::vector<Module*> modules;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	char buf1[128] = "Engine 3D - Marc Latorre && Marti Majo";
	char buf2[128] = "UPC CITM";
	bool conf_active = true;

public:

	Application();
	~Application();

	bool Init();
	UPDATE_STATUS Update();
	bool CleanUp();

	void OpenWebsite(const char* url);

	const std::string GetTitle() const;
	const std::string GetOrganization() const;
	void OpenCloseConfigWindow();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	UPDATE_STATUS CreateConfigMenu();
	UPDATE_STATUS EndConfigMenu();
};

#endif //_APPLICATION