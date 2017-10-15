#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
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

void GameObject::Configuration() const
{
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
	if (this != nullptr)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
			delete *it;

		components.clear();
	}
}

void GameObject::ReserveComponentSpace(const GLuint & num_components)
{
	components.reserve(num_components * sizeof(Component*));
}

void GameObject::GenerateBoundingBox(AABB & bounding_box) const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == CT_MESH)
		{
			((Mesh*)(*it))->Enclose(bounding_box);
		}
	}
}
void GameObject::GetWorldPosition(GLfloat & x, GLfloat & y, GLfloat & z)
{
	x = world_position[0];
	y = world_position[1];
	z = world_position[2];
}
