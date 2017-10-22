#ifndef _APPLICATION
#define _APPLICATION

#include <vector>
#include <string>
#include "MathGeoLib\src\Time\Clock.h"
#include "Globals.h"
#include "Module.h"

#define FPS_GRAPH_SIZE 30

class Window;
class Input;
class Audio;
class Renderer3D;
class Camera3D;
class Console;
class Hardware;
class MainMenuBar;
class HardwareSoftwareInfo;
class BasicGeometry;
class SceneManager;

namespace ImGui
{
	struct DockContext;
}

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
	BasicGeometry* primitives;
	SceneManager* scene_manager;

private:
	std::string title;
	std::string organization;
	float	dt;
	std::vector<Module*> modules;
	float fps_log[FPS_GRAPH_SIZE];
	float ms_log[FPS_GRAPH_SIZE];
	char buf1[128] = "Engine 3D - Marc Latorre && Marti Majo";
	char buf2[128] = "UPC CITM";
	bool conf_active = true;
	ImGui::DockContext* app_dock = nullptr;

public:

	Application();
	~Application();

	bool Init();
	UPDATE_STATUS Update();
	bool CleanUp();

	void OpenWebsite(const char* url);

	const char* GetTitle() const;
	const char* GetOrganization() const;
	void OpenCloseConfigWindow();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	UPDATE_STATUS CreateConfigMenu();
	UPDATE_STATUS EndConfigMenu();
};

// Give App pointer access everywhere
extern Application* App;

#endif //_APPLICATION