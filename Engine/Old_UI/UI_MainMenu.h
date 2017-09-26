#ifndef _UI_MAIN_MENU
#define _UI_MAIN_MENU

#include "UI_Element.h"

class UI_MainMenu : public UI_Element
{
private:
	ModuleGUI* gui = nullptr;

public:
	UI_MainMenu(ModuleGUI* gui, bool active = true);
	~UI_MainMenu();

	bool Update();
};

#endif // !_UI_MAIN_MENU

