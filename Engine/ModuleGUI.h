#ifndef _MODULE_GUI
#define _MODULE_GUI

#include "Module.h"

enum WEEL_MOVEMENT
{	
	WM_DOWN = -1,
	WM_NO_MOVEMENT = 0,
	WM_UP = 1
};

class ModuleGUI : public Module
{
private:
	double g_time = 0.0f;
	WEEL_MOVEMENT g_mouse_weel = WM_NO_MOVEMENT;

public:

	ModuleGUI(Application* app, bool start_enabled = true);
	~ModuleGUI();

	bool Init();

	UPDATE_STATUS Update(float dt);

	void UpdateGUIInputs(float dt);

	void SetMouseWeel(WEEL_MOVEMENT movement);
};

#endif // _MODULE_GUI
