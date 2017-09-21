#ifndef _MODULE_GUI
#define _MODULE_GUI

#include "Module.h"
#include "imgui-master\imgui.h"

enum UI_TYPE;
class UI_Element;
class UI_Test;
class UI_Rand_Test;
class UI_MainMenu;
class UI_GeometryCollisionTest;

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

	bool show_test_window = true;
	bool show_menu_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	std::vector<UI_Element*> ui_elements;

	bool CreateMainMenuBar();

public:

	ModuleGUI(Application* app, bool start_enabled = true);
	~ModuleGUI();

	bool Init();

	UPDATE_STATUS Update(float dt);

	bool CleanUp();

	void PreRender();

	UI_Test* CreateTestMenu(bool active = true);
	UI_Rand_Test* CreateRandomNumMenu(bool active = true);
	UI_MainMenu* CreateMainMenu(bool active = true);
	UI_GeometryCollisionTest* CreateGeometryTest(bool active = true);

	void SwitchActivation(UI_TYPE element);
	void Activate(UI_TYPE element);
	bool GetActive(UI_TYPE element) const;
	void Deactivate(UI_TYPE element);
};

#endif // _MODULE_GUI
