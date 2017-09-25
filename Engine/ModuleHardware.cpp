#include "imgui\imgui.h"
#include "SDL\include\SDL.h"
#include "gpudetect\DeviceId.h"
#include "glut\glut.h"
#include "ModuleHardware.h"

ModuleHardware::ModuleHardware(Application * app, bool start_enabled): Module(app, "Hardware", start_enabled)
{}

ModuleHardware::~ModuleHardware()
{}

bool ModuleHardware::Start()
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
	// With this program crashes cus of not finding entry point
	if (SDL_HasAVX2())
		caps += "AVX2";
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

	ogl = *glGetString(GL_VERSION);

	//new
	unsigned long long video_memory;
	getGraphicsDeviceInfo(&vendor_id, &device_id, &video_memory, &gfx_brand);
	vram = (float)video_memory / (1024.f * 1024.f * 1024.f);

	return ret;
}

UPDATE_STATUS ModuleHardware::Configuration()
{
	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("HardWare"))
	{
		ImGui::Text("SDL Version %i.%i.%i", version.major, version.minor, version.patch);
		ImGui::Separator();
		ImGui::Text("CPUs: %i (cache:%i)", cpu_count, cpu_cache);
		ImGui::Text("System RAM: %.3f Gb", sys_ram);
		ImGui::Text("Caps: ");
		ImGui::SameLine();
		ImGui::Text(caps.c_str());
		ImGui::Separator();
		ImGui::Text("OpenGL Version %c", ogl);
		ImGui::Separator();

		ImGui::Text("Vendor %i device: %i", vendor_id, device_id);
		char char_array[250];
		sprintf_s(char_array, 250, "%S", gfx_brand.c_str());
		brand += char_array;
		ImGui::Text("Brand:");
		ImGui::SameLine();
		ImGui::Text(brand.c_str());
		ImGui::Text("VRAM: %f", vram);
	}
	return ret;
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
		std::string brand("Brand: ");
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

