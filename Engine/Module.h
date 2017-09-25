#ifndef _MODULE
#define _MODULE

#include <string>
#include "Globals.h"
#include "MathGeoLib\src\Time\Clock.h"

class Application;

class Module
{
private :
	bool enabled;

public:
	std::string name;
	Application* App;

	Module(Application* parent,const char* name, bool start_enabled = true) : App(parent), name(name)
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

	UPDATE_STATUS PreUpdateWithTimer(float dt, const Clock& c, int& ms)
	{
		UPDATE_STATUS ret = UPDATE_CONTINUE;
		int start = c.Time();
		ret = PreUpdate(dt);
		int end = c.Time();
		ms = end - start;
		return ret;
	}

	virtual UPDATE_STATUS Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	UPDATE_STATUS UpdateWithTimer(float dt, const Clock& c, int& ms)
	{
		UPDATE_STATUS ret = UPDATE_CONTINUE;
		int start = c.Time();
		ret = Update(dt);
		int end = c.Time();
		ms = end - start;
		return ret;
	}

	virtual UPDATE_STATUS PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	UPDATE_STATUS PostUpdateWithTimer(float dt, const Clock& c, int& ms)
	{
		UPDATE_STATUS ret = UPDATE_CONTINUE;
		int start = c.Time();
		ret = PostUpdate(dt);
		int end = c.Time();
		ms = end - start;
		return ret;
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