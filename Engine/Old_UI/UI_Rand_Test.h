#ifndef _UI_RAND_TEST
#define _UI_RAND_TEST

#include "UI_Element.h"

class UI_Rand_Test : public UI_Element
{
private:
	int num0_0, num0_1, num0_2, num0_3, num0_4, num0_5, num0_6, num0_7, num0_8, num0_9, num1_0;

	int GetNum(int i);

public:
	UI_Rand_Test(bool active = true);
	~UI_Rand_Test();

	bool Update();
};

#endif //_UI_RAND_TEST