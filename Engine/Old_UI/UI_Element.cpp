#include "UI_Element.h"

UI_Element::UI_Element(UI_TYPE type, bool active): type(type), active(active)
{}

UI_Element::~UI_Element()
{}

bool UI_Element::Active()
{
	return active;
}

void UI_Element::Activate()
{
	active = true;
}

void UI_Element::Deactivate()
{
	active = false;
}

UI_TYPE UI_Element::GetType() const
{
	return type;
}
