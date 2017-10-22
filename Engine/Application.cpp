#include <string>
#include "glew\include\GL\glew.h"
#include "Parson\parson.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "imgui\imgui_dock.h"
#include "Brofiler\Brofiler.h"
#include "Window.h"
#include "Input.h"
#include "Audio.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "Console.h"
#include "HardwareSoftwareInfo.h"
#include "MainMenuBar.h"
#include "TextureInporter.h"
#include "SceneInporter.h"
#include "BasicGeometry.h"
#include "SceneManager.h"
#include "Application.h"

Application::Application()
{
	window = new Window("Window");
	input = new Input("Input");
	audio = new Audio("Audio");
	renderer_3d = new Renderer3D("Renderer3D");
	camera = new Camera3D("Camera3D");
	console = new Console("Console");
	hardware_software_info = new HardwareSoftwareInfo("Hardware & Software");
	main_menu_bar = new MainMenuBar("Menu Bar");
	primitives = new BasicGeometry("Basic Geometry");
	scene_manager = new SceneManager("Scene Manager");

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order
	
	// Main Modules
	AddModule(window);
	AddModule(console);
	AddModule(input);
	AddModule(audio);
	AddModule(camera);
	AddModule(hardware_software_info);
	AddModule(main_menu_bar);
	AddModule(primitives);
	AddModule(scene_manager);
	
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

	app_dock = new ImGui::DockContext();



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
	dt = 1.0f / ImGui::GetIO().Framerate;

	ImGui_ImplSdlGL2_NewFrame(window->window);
}

// ---------------------------------------------
void Application::FinishUpdate()
{}
 
UPDATE_STATUS Application::CreateConfigMenu()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	/*
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(App->window->GetWidth() * 0.5f, 25), ImGuiCond_Always); 
	
	ImGui::Begin("Docking Test##config", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::Text("Dock Test");

	//ImGui::BeginChild("Docking Test", ImVec2(200, 200));

	bool application_dock = true;
	app_dock->BeginDockContext("Docking Context test", ImVec2(App->window->GetWidth() * 0.5f, 25), ImVec2(400, 300), &application_dock);
	app_dock->EndDockContext();

	if (app_dock->BeginDock("1 window", &application_dock, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse))
		ImGui::Text("Im window number 1");
	app_dock->EndDock();
/*
	if(app_dock->BeginDock("2 window", &application_dock, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse))
		ImGui::Text("Im window number 2");
	app_dock->EndDock();

	if(app_dock->BeginDock("3 window", &application_dock, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse))
		ImGui::Text("Im window number 3");
	app_dock->EndDock();
	*/
	//ImGui::EndChild();
	//ImGui::End();
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

			//FPS graphic
			for (uint i = 0; i < FPS_GRAPH_SIZE; i++)
				fps_log[i] = fps_log[i + 1];

			fps_log[FPS_GRAPH_SIZE - 1] = ImGui::GetIO().Framerate;

			char fps_title[25];
			sprintf_s(fps_title, 25, "Framerate %.1f", fps_log[29]);
			ImGui::PlotHistogram("", fps_log, IM_ARRAYSIZE(fps_log), FPS_GRAPH_SIZE, fps_title, 0.0f, 80.0f, ImVec2(0, 100));

			//Framerate graphic
			for (uint i = 0; i < FPS_GRAPH_SIZE; i++)
				ms_log[i] = ms_log[i + 1];

			ms_log[FPS_GRAPH_SIZE - 1] = dt * 1000;

			//Blit milliseconds graphic
			char ms_title[25];
			sprintf_s(ms_title, 25, "Milliseconds %.1f", ms_log[29]);
			ImGui::PlotHistogram("", ms_log, IM_ARRAYSIZE(ms_log), FPS_GRAPH_SIZE, ms_title, 0.0f, 80.0f, ImVec2(0, 100));

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

	DELETE_PTR(app_dock);

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