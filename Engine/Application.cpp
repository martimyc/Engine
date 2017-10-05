#include <string>
#include "glew\include\GL\glew.h"
#include "Parson\parson.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Window.h"
#include "Input.h"
#include "Audio.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "Console.h"
#include "HardwareSoftwareInfo.h"
#include "MainMenuBar.h"
#include "OpenGLTest.h"
#include "Application.h"


Application::Application()
{
	window = new Window(this);
	input = new Input(this);
	audio = new Audio(this, true);
	renderer_3d = new Renderer3D(this);
	camera = new Camera3D(this);
	console = new Console(this);
	hardware_software_info = new HardwareSoftwareInfo(this);
	main_menu_bar = new MainMenuBar(this);
	open_gl_test = new OpenGLTest(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order
	
	// Main Modules
	AddModule(console);	//console first for init logs
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(camera);
	AddModule(hardware_software_info);
	AddModule(main_menu_bar);
	AddModule(open_gl_test);
	
	// Scenes

	// Renderer last!
	AddModule(renderer_3d);
}

Application::~Application()
{
	for(std::vector<Module*>::const_reverse_iterator it = modules.rbegin(); it != modules.rend(); ++it)
		delete *it;
}

bool Application::Init()
{
	bool ret = true;
	
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* app = json_object_dotget_object(obj, "App"); 
	title = json_object_get_string(app, "title");
	organization = json_object_get_string(app, "organization");

	std::vector<Module*>::const_iterator it = modules.begin();

	// Call Init() in all modules
	for(; it != modules.end() && ret == true; ++it)
		ret = (*it)->Init();

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	
	for(it = modules.begin(); it != modules.end() && ret == true; ++it)
		ret = (*it)->Start();

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (ms_timer.Time() - last_frame_time) / 1000.0f;
	last_frame_time = ms_timer.Time();

	ImGui_ImplSdlGL2_NewFrame(window->window);
}

// ---------------------------------------------
void Application::FinishUpdate()
{}

UPDATE_STATUS Application::CreateConfigMenu()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::Begin("Configuration", &conf_active))
	{

		if (ImGui::CollapsingHeader("Application"))
		{
			JSON_Value* config = json_parse_file("config.json");
			JSON_Object* obj = json_value_get_object(config);
			JSON_Object* app_obj = json_object_dotget_object(obj, "App");
			JSON_Value* app = json_value_init_object();

			if (ImGui::InputText("Engine name", buf1, 128))
				title = buf1;
			json_object_set_string(json_object(app), "title", title.c_str());
			json_object_dotset_value(obj, "App", app);

			if (ImGui::InputText("Organization", buf2, 128))
				organization = buf2;
			json_object_set_string(json_object(app), "organization", organization.c_str());
			json_object_dotset_value(obj, "App", app);

			json_serialize_to_file(config, "config.json");



			if (fps_log.size() > 0 && ms_log.size() > 0)
			{
				char title[25];
				sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
				ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
				sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
				ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
			}
		}
	}
	return ret;
}

UPDATE_STATUS Application::EndConfigMenu()
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	return UPDATE_CONTINUE;
}

// Call PreUpdate, Update and PostUpdate on all modules
UPDATE_STATUS Application::Update()
{
	BROFILER_CATEGORY("Aplication Update", Profiler::Color::LightYellow)

	UPDATE_STATUS ret = UPDATE_CONTINUE;
	PrepareUpdate();

	std::vector<Module*>::const_iterator it = modules.begin();

	for (; it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate(dt);

	//Configuration menu
	if (conf_active)
	{
		if (ret == UPDATE_CONTINUE)
			ret = CreateConfigMenu();

		for (it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->Configuration(dt);

		if (ret == UPDATE_CONTINUE)
			ret = EndConfigMenu();
	}
	//--

	for (it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update(dt);

	for (it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate(dt);

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::vector<Module*>::const_reverse_iterator it = modules.rbegin(); it != modules.rend() && ret == true; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::OpenWebsite(const char * url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

const char* Application::GetTitle() const
{
	return title.c_str();
}

const char* Application::GetOrganization() const
{
	return organization.c_str();
}

void Application::OpenCloseConfigWindow()
{
	conf_active = !conf_active;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}