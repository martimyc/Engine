#include "SDL2\include\SDL.h"
#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "gpudetect\DeviceId.h"
#include "Brofiler\Brofiler.h"
#include "Globals.h"
#include "Application.h"
#include "Console.h"
#include "HardwareSoftwareInfo.h"

HardwareSoftwareInfo::HardwareSoftwareInfo(const char* name, bool start_enabled): Module(name, start_enabled)
{}

HardwareSoftwareInfo::~HardwareSoftwareInfo()
{}

bool HardwareSoftwareInfo::Start()
{
	bool ret = true;

	SDL_GetVersion(&version);
	cpu_count = SDL_GetCPUCount();
	cpu_cache = SDL_GetCPUCacheLineSize();
	sys_ram = (float)SDL_GetSystemRAM() / 1000.0f;

	if (SDL_Has3DNow())
		caps += "3DNow, ";
	if (SDL_HasAVX())
		caps += "AVX, ";
	// With this program crashes cus of not finding entry point only in debug
	//if (SDL_HasAVX2())
		//caps += "AVX2";
	if (SDL_HasAltiVec())
		caps += "AltiVec, ";
	if (SDL_HasMMX())
		caps += "MMX, ";
	if (SDL_HasRDTSC())
		caps += "RDTSC, ";
	if (SDL_HasSSE())
		caps += "SSE, ";
	if (SDL_HasSSE2())
		caps += "SSE 2, ";
	if (SDL_HasSSE3())
		caps += "SSE 3, ";
	if (SDL_HasSSE41())
		caps += "SSE 4.1, ";
	if (SDL_HasSSE42())
		caps += "SSE 4.2";

	glew_version = glewGetString(GLEW_VERSION);

	gl_vendor = glGetString(GL_VENDOR);
	gl_renderer = glGetString(GL_RENDERER);
	gl_version = glGetString(GL_VERSION);
	gl_shading_language_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	//new
	unsigned long long video_memory;
	if (!getGraphicsDeviceInfo(&vendor_id, &device_id, &video_memory, &gfx_brand))
	{
		gpudetect_new = false;

		unsigned long long video_memory_budget;
		unsigned long long video_memory_usage;
		unsigned long long video_memory_available;
		unsigned long long video_memory_reserved;

		if (!getGraphicsDeviceInfo(&vendor_id, &device_id, &gfx_brand, &video_memory_budget, &video_memory_usage, &video_memory_available, &video_memory_reserved))
		{
			
			LOG("Can not identify GPU and VRAM");
		}
		else
		{
			vram_budget = (float)video_memory_budget / (1024.f * 1024.f * 1024.f);
			vram_usage = (float)video_memory_usage / (1024.f * 1024.f * 1024.f);
			vram_available = (float)video_memory_available / (1024.f * 1024.f * 1024.f);
			vram_reserved = (float)video_memory_reserved / (1024.f * 1024.f * 1024.f);
		}
	}
	else
		vram = (float)video_memory / (1024.f * 1024.f * 1024.f);

	config_hardware_software = false;

	return ret;
}

UPDATE_STATUS HardwareSoftwareInfo::Configuration(float dt)
{
	BROFILER_CATEGORY("Hardware Configuration", Profiler::Color::BlanchedAlmond)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (App->BeginDockWindow("HardWare", &config_hardware_software))
	{
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "CPU");
		ImGui::Text("CPUs: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", cpu_count);
		ImGui::SameLine(); 
		ImGui::Text("(cache:");
		ImGui::SameLine(); 
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i)", cpu_cache);
		ImGui::Text("System RAM: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%.3f Gb", sys_ram);
		ImGui::Text("Caps: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255),caps.c_str());
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SDL, OpenGL & Glew");
		ImGui::Text("SDL Version: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i.%i.%i", version.major, version.minor, version.patch);
		ImGui::Text("Glew Version:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", glew_version);
		ImGui::Text("OpenGL Vendor: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", gl_vendor);
		ImGui::Text("OpenGL Renederer: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", gl_renderer);
		ImGui::Text("OpenGL Version: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", gl_version);
		ImGui::Text("OpenGL Shading Language Version: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", gl_shading_language_version);
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "GPU");
		if (gpudetect_new)
		{
			ImGui::Text("Vendor: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i ", vendor_id);
			ImGui::SameLine();
			ImGui::Text("device: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", device_id);
			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "%S", gfx_brand.c_str());
			ImGui::Text("VRAM: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", vram);
		}
		else
			if (gpudetect_old)
			{
				ImGui::Text("Vendor: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i ", vendor_id);
				ImGui::SameLine(); 
				ImGui::Text("device: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", device_id);
				ImGui::Text("Brand:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%S", gfx_brand.c_str());
				ImGui::Text("VRAM budget: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", vram_budget);
				ImGui::Text("VRAM usage: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", vram_usage);
				ImGui::Text("VRAM available: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", vram_available);
				ImGui::Text("VRAM reserved: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", vram_reserved);
			}
			else
				ImGui::Text("Could not detect GPU & VRAM properlly");
		ImGui::Separator();
	}
	App->EndDockWindow();

	return ret;
}

void HardwareSoftwareInfo::OpenCloseConfigHardwareSoftwareWindow()
{
	config_hardware_software = !config_hardware_software;
}

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

		/*
		unsigned int vendor_id;
		unsigned int device_id;
		unsigned long long video_memory_budget;
		unsigned long long video_memory_usage;
		unsigned long long video_memory_available;
		unsigned long long video_memory_reserved;
		std::wstring gfx_brand;

		if (getGraphicsDeviceInfo(&vendor_id, &device_id, &gfx_brand, &video_memory_budget, &video_memory_usage, &video_memory_available, &video_memory_reserved))
		{
		ImGui::Text("Vendor %i device: %i", vendor_id, device_id);
		char char_array[250];
		sprintf_s(char_array, 250, "%S", gfx_brand.c_str());
		brand += char_array;
		ImGui::Text("Brand: %s", brand.c_str());
		ImGui::Text("VRAM Budget: %f", (float)video_memory_budget / 1073741824.0f);
		ImGui::Text("VRAM Usage: %f", (float)video_memory_usage / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Available: %f", (float)video_memory_available / (1024.f * 1024.f * 1024.f));
		ImGui::Text("VRAM Reserved: %f", (float)video_memory_reserved / (1024.f * 1024.f * 1024.f));
		}*/

