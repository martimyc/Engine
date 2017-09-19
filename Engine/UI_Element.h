#ifndef _UI_ELEMENTS
#define _UI_ELEMENTS

enum UI_TYPE
{
	UI_NO_ELEMENT = 0,
	UI_TEST,
	UI_MAINMENU,
	UI_RAND_TEST
};

class UI_Element
{
protected:
	UI_TYPE type;
	bool active;

public:
	UI_Element(UI_TYPE type, bool active = true);
	~UI_Element();

	bool Active();
	void Activate();
	void Deactivate();

	UI_TYPE GetType() const;

	virtual bool Update() = 0;
};

#endif //_UI_ELEMENTS
