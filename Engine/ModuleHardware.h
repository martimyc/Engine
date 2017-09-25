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
	GLubyte ogl;
	unsigned int vendor_id;
	unsigned int device_id;
	float vram;
	std::string brand;
	std::wstring gfx_brand;

public:
	ModuleHardware(Application* app, bool start_enabled = true);
	~ModuleHardware();

	bool Start();

	UPDATE_STATUS Configuration();

};

#endif // !_MODULE_HARDWARE

