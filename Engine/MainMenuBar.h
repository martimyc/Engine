#ifndef _MODULE_MAIN_MENU_BAR
#define _MODULE_MAIN_MENU_BAR

#include "Module.h"

enum UI_TYPE;
class UI_Element;
class UI_Test;
class UI_Rand_Test;
class UI_MainMenu;
class UI_GeometryCollisionTest;


class MainMenuBar : public Module
{
private:
	bool show_about = false;

public:

	MainMenuBar(const char* name, bool start_enabled = true);
	~MainMenuBar();

	UPDATE_STATUS Update(float dt);

};

#endif // _MODULE_MAIN_MENU_BAR
