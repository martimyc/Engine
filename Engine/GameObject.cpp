#include "Mesh.h"
#include "Console.h"
#include "Material.h"
#include "Texture.h"
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

void GameObject::ApplyTexture(Texture * text)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if((*it)->Enabled() && (*it)->GetType() == CT_MESH)
		{
			Material* material = ((Mesh*)(*it))->GetMaterial();
			if (material != nullptr)
			{
				material->Empty();
				material->AddTexture(text);
			}
			else
			{
				material = new Material();
				material->AddTexture(text);
			}
		}
	}
}

void GameObject::Reset()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;

	components.clear();
}

void GameObject::ReserveComponentSpace(const GLuint & num_components)
{
	components.reserve(num_components * sizeof(Component*));
}
