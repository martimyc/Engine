#include <string>
#include "glut\glut.h"
//#include "gpudetect\DeviceId.h"
#include "gpudetect\old\DeviceId.h"
#include "Parson\parson.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "Application.h"


Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer_3d = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGUI(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(camera);
	AddModule(gui);
	
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
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	ImGui_ImplSdlGL2_NewFrame(window->window);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	float fps = ImGui::GetIO().Framerate;
	float ms = 1000.0f / fps;

	if (fps_cap != 0)
	{
		float cap_time = 1000.0f / fps_cap;
		float wait = ms - cap_time;

		if (wait < 0.0f)
		{
			LOG("Running below desired fps");
		}
		else
			Sleep(wait);
	}

	ms_log.push_back(ms);
	fps_log.push_back(fps);
}

UPDATE_STATUS Application::CreateConfigMenu()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	ImGui::Begin("Configuration");

	if (fps_log.size() > 0 && ms_log.size() > 0)
	{
		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	return ret;
}

UPDATE_STATUS Application::EndConfigMenu()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	ImGui::CollapsingHeader("HardWare");

	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::Text("SDL Version %i.%i.%i", version.major, version.minor, version.patch);

	ImGui::Separator();

	ImGui::Text("CPUs: %i (cache:%i)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	ImGui::Text("System RAM: %.3f Gb", (float)SDL_GetSystemRAM() / 1000.0f);

	std::string caps("Caps: ");

	if (SDL_Has3DNow())
		caps += "3DNow, ";
	if(SDL_HasAVX())
		caps += "AVX, ";
	// With this program crashes cus of not finding entry point
	//if (SDL_HasAVX2())
		//caps += "AVX2";
	if(SDL_HasAltiVec())
		caps += "AltiVec, ";
	if(SDL_HasMMX())
		caps += "MMX, ";
	if(SDL_HasRDTSC())
		caps += "RDTSC, ";
	if(SDL_HasSSE())
		caps += "SSE, ";
	if(SDL_HasSSE2())
		caps += "SSE 2, ";
	if(SDL_HasSSE3())
		caps += "SSE 3, ";
	if(SDL_HasSSE41())
		caps += "SSE 4.1, ";
	if(SDL_HasSSE42())
		caps += "SSE 4.2";

	ImGui::Text(caps.c_str());

	ImGui::Separator();

	ImGui::Text("OpenGL Version %c", glGetString(GL_VERSION));

	ImGui::Separator();

	//new
	/*unsigned int vendor_id;
	unsigned int device_id;
	unsigned long long video_memory;
	std::string brand("Brand: ");
	std::wstring gfx_brand;
	
	if (getGraphicsDeviceInfo(&vendor_id, &device_id,  &video_memory, &gfx_brand))
	{
		ImGui::Text("Vendor %i device: %i", vendor_id, device_id);
		char char_array[250];
		sprintf_s(char_array, 250, "%S", gfx_brand.c_str());
		brand += char_array;
		ImGui::Text("Brand: %s", brand.c_str());
		ImGui::Text("VRAM: %i", video_memory);
	}*/

	unsigned int vendor_id;
	unsigned int device_id;
	unsigned long long video_memory_budget;
	unsigned long long video_memory_usage;
	unsigned long long video_memory_available;
	unsigned long long video_memory_reserved;
	std::string brand("Brand: ");
	std::wstring gfx_brand;

	if (getGraphicsDeviceInfo(&vendor_id, &device_id, &gfx_brand, &video_memory_budget, &video_memory_usage, &video_memory_available, &video_memory_reserved))
	{
		ImGui::Text("Vendor %i device: %i", vendor_id, device_id);
		char char_array[250];
		sprintf_s(char_array, 250, "%S", gfx_brand.c_str());
		brand += char_array;
		ImGui::Text("Brand: %s", brand.c_str());
		ImGui::Text("VRAM Budget: %f", (float) video_memory_budget / 1073741824.0f);
		ImGui::Text("VRAM Usage: %f", (float)video_memory_usage / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Available: %f", (float)video_memory_available / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Reserved: %f", (float)video_memory_reserved / (1024.f * 1024.f * 1024.f));
	}

	/*if (getGraphicsDeviceInfo(&vendor, &device_id, &brand, &video_mem_budget, &video_mem_usage, &video_mem_available, &video_mem_reserved))
	{
		ImGui::Text("Vendor %i device: %i", vendor, device_id);
		std::string brand_name("Brand: ");
		char char_array[250];
		sprintf_s(char_array, 250, "%S", brand.c_str());
		brand_name += char_array;
		ImGui::Text(brand_name.c_str());
		ImGui::Text("VRAM Budget: %.1f", float(video_mem_budget) / 1073741824.0f);
		ImGui::Text("VRAM Usage: %.1f", float(video_mem_usage) / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Available: %.1f", float(video_mem_available) / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Reserved: %.1f", float(video_mem_reserved) / (1024.f * 1024.f * 1024.f));

		// I don't understean this
		info.vram_mb_budget = float(video_mem_budget) / 1073741824.0f;
		info.vram_mb_usage = float(video_mem_usage) / (1024.f * 1024.f * 1024.f);
		info.vram_mb_available = float(video_mem_available) / (1024.f * 1024.f * 1024.f);
		info.vram_mb_reserved = float(video_mem_reserved) / (1024.f * 1024.f * 1024.f);
	}*/

	ImGui::End();
	return ret;
}

// Call PreUpdate, Update and PostUpdate on all modules
UPDATE_STATUS Application::Update()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;
	PrepareUpdate();

	std::vector<Module*>::const_iterator it = modules.begin();

	//Configuration menu
	ret = CreateConfigMenu();

	for (; it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Configuration(dt);

	ret = EndConfigMenu();
	//--

	for(it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate(dt);

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