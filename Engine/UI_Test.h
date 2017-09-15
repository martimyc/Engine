#ifndef _UI_TEST
#define _UI_TEST

#include "UI_Element.h"

class UI_Test : public UI_Element
{
public:
	UI_Test(bool active = true);
	~UI_Test();

	bool Update();
};

#endif //_UI_TEST