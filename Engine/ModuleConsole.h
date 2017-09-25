#ifndef _MODULE_CONSOLE
#define _MODULE_CONSOLE

#include <deque>
#include <string>
#include "Module.h"

#define MAX_LOGS 10

class ModuleConsole : public Module
{
private: 
	std::deque<std::string> log_vec;
public:
	ModuleConsole(Application* parent, bool start_enabled = true);
	~ModuleConsole();

	UPDATE_STATUS Update(float dt);
	
	void Log(const char file[], int line, const char* format, ...);
};

#endif // _MODULE_CONSOLE