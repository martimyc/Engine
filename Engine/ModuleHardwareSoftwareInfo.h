#ifndef _MODULE_HARDWARE_SOFTWARE_INFO
#define _MODULE_HARDWARE_SOFTWARE_INFO

#include <string>
#include "Module.h"

class ModuleHardware : public Module
{
private:
	SDL_version version;
	std::string caps;
	unsigned int cpu_count = 0;
	unsigned int cpu_cache = 0;
	float sys_ram = 0.0f;
	const GLubyte* glew_version;
	const GLubyte* gl_vendor;
	const GLubyte* gl_renderer;
	const GLubyte* gl_version;
	const GLubyte* gl_shading_language_version;
	bool gpudetect_new = true;
	bool gpudetect_old = true;
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

#endif // !_MODULE_HARDWARE_INFO

