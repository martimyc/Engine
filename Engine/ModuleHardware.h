#ifndef _MODULE_HARDWARE
#define _MODULE_HARDWARE

#include "Module.h"

class ModuleHardware : public Module
{
private:
	SDL_version version;
	std::string caps;
	unsigned int cpu_count = 0;
	unsigned int cpu_cache = 0;
	float sys_ram = 0.0f;
	GLubyte gl_vendor;
	GLubyte gl_renderer;
	GLubyte gl_version;
	GLubyte gl_shading_language_version;
	bool gpudetect = true;
	unsigned int vendor_id;
	unsigned int device_id;
	//new
	float vram;
	//old (only if new doesnt work)
	float vram_budget;
	float vram_usage;
	float vram_available;
	float vram_reserved;
	std::wstring gfx_brand;

public:
	ModuleHardware(Application* app, bool start_enabled = true);
	~ModuleHardware();

	bool Start();

	UPDATE_STATUS Configuration(float dt);

};

#endif // !_MODULE_HARDWARE

