#include <string>
#include "glut\glut.h"
#include "Parson\parson.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleConsole.h"
#include "ModuleHardware.h"
#include "MainMenuBar.h"
#include "Application.h"


Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer_3d = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	console = new ModuleConsole(this);
	hardware = new ModuleHardware(this);
	main_menu_bar = new ModuleMainMenuBar(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order
	
	// Main Modules
	AddModule(console);	//console first for init logs
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(camera);
	AddModule(hardware);
	AddModule(main_menu_bar);
	
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
	fps_cap = json_object_get_number(app, "max_fps");

	/*
	//Charge config	 TEST
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);	//Gets starting doc
	JSON_Object* app = json_object_dotget_object(obj, "App"); //Gets App son
	std::string name = json_object_get_string(app, "name");	//Gets name from App
	int max_fps = json_object_get_number(app, "max_fps"); //Gets max_fps from App

														  
	//Save
	float f = 28.3;
	JSON_Value* test = json_value_init_object();
	json_object_set_string(json_object(test), "Test", "");
	json_object_dotset_value(obj, "Test2", test);
	json_serialize_to_file(config, "config.json");
	//------------------
	*/

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
{
	//TODO cap fps (done whreng here)
	/*float fps = ImGui::GetIO().Framerate;
	float ms = 1000.0f / fps;

	if (fps_cap != 0)
	{
		float cap_time = 1000.0f / fps_cap;
		float wait = cap_time - ms;

		if (wait < 0.0f)
		{
			LOG("Running below desired fps");
		}
		else
			ms_timer.Sleep(wait);
	}*/	
}

UPDATE_STATUS Application::CreateConfigMenu()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	ImGui::Begin("Configuration");

	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::InputText("Engine name", buf1, 128);		
		title = buf1;
	
		ImGui::InputText("Organization", buf2, 128);
		organization = buf2;

		if (ImGui::Button("Update Engine name/organization"))
		{
			/*JSON_Value* config = json_parse_file("config.json");
			JSON_Value* new_title = json_value_init_object();
			JSON_Object* obj = json_value_get_object(config);
			JSON_Object* app = json_object_dotget_object(obj, "App");
			json_object_set_string(json_object(new_title), "title", title.c_str());
			json_object_dotset_value(obj, "App", new_title);
			json_serialize_to_file(config, "config.json");
			window->SetTitle(title.c_str());
			*/
		}

		if (fps_log.size() > 0 && ms_log.size() > 0)
		{
			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
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
	UPDATE_STATUS ret = UPDATE_CONTINUE;
	PrepareUpdate();

	std::vector<Module*>::const_iterator it = modules.begin();

	for (; it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate(dt);
	
	//Configuration menu
	if(ret == UPDATE_CONTINUE)
		ret = CreateConfigMenu();

	for (it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Configuration(dt);

	if (ret == UPDATE_CONTINUE)
		ret = EndConfigMenu();
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

const std::string Application::GetTitle() const
{
	return title;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}