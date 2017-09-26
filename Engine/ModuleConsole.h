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
	bool console_active = true;
public:
	ModuleConsole(Application* parent, bool start_enabled = true);
	~ModuleConsole();

	UPDATE_STATUS Update(float dt);
	
	void Log( const char* format, ...);
	void OpenCloseConsoleWindow();
};

#endif // _MODULE_CONSOLE