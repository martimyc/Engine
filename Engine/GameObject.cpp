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
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete (*it);
	components.clear();
}

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
	if (components.size() > 0)
	{
		ImGui::Begin("Inspector");

		for (int i = 0; i < components.size(); i++)
			components[i]->Configuration(i);

		ImGui::End();
	}
}

void GameObject::ApplyTexture(Texture * text)
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->Enabled() && (*it)->GetType() == CT_MESH)
			((Mesh*)(*it))->ApplyTexture(text);
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

void GameObject::ChangeMaterial(Material * new_material, int mesh_num)
{
	int current_mesh = 0;
	bool found = false;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == CT_MESH)
			current_mesh++;
		if (current_mesh == mesh_num)
		{
			((Mesh*)(*it))->ChangeMaterial(new_material);
			found = true;
		}
	}

	if (!found)
		LOG("Mesh not found");
}
