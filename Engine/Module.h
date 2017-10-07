#ifndef _MODULE
#define _MODULE

#include <string>
#include "Globals.h"

class Application;

class Module
{
private :
	bool enabled;

public:
	std::string name;

	Module(const char* name, bool start_enabled = true) : name(name), enabled(start_enabled)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual UPDATE_STATUS PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual UPDATE_STATUS Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual UPDATE_STATUS PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual UPDATE_STATUS Configuration(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};

#endif //_MODULE