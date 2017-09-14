#ifndef _MODULE_GUI
#define _MODULE_GUI

#include "Module.h"

class ModuleGUI : public Module
{
private:
	double g_time = 0.0f;

public:

	bool Init();

	UPDATE_STATUS Update(float dt);
};

#endif // _MODULE_GUI
