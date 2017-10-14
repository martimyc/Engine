
#include "Mesh.h"
#include "Console.h"
#include "GameObject.h"

GameObject::GameObject()
{}

GameObject::~GameObject()
{}

void GameObject::Draw() const
{
	if (components.size() > 0)
	{
		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
			if ((*it)->Enabled())
				(*it)->Draw();
	}		
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}

void GameObject::Reset()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;

	components.clear();
}
