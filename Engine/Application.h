#ifndef _APPLICATION
#define _APPLICATION

#include <vector>
#include <string>
#include "MathGeoLib\src\Time\Clock.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"

class Window;
class Input;
class Audio;
class Renderer3D;
class Camera3D;
class Console;
class Hardware;
class MainMenuBar;
class HardwareSoftwareInfo;
class OpenGLTest;

class Application
{
public:
	Window* window;
	Input* input;
	Audio* audio;
	Renderer3D* renderer_3d;
	Camera3D* camera;
	Console* console;
	Hardware* hardware;
	MainMenuBar* main_menu_bar;
	HardwareSoftwareInfo* hardware_software_info;
	OpenGLTest* open_gl_test;

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