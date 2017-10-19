#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Mesh.h"
#include "Console.h"
#include "Material.h"
#include "Texture.h"
#include "GameObject.h"

GameObject::GameObject(const TreeNode* const node, const char* name): tree_node(node), name(name)
{}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete (*it);
	components.clear();
}

void GameObject::Draw(bool normals) const
{
	if (components.size() > 0)
	{
		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
			if ((*it)->Enabled())
				(*it)->Draw(normals);
	}		
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}

void GameObject::Inspector() const
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

void GameObject::ChangeMaterial(Material * new_material, int mesh_num)
{
	int current_mesh = 0;
	bool found = false;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == CT_MESH)
		{
			if (current_mesh == mesh_num)
			{
				((Mesh*)(*it))->ChangeMaterial(new_material);
				found = true;
			}
			current_mesh++;
		}
	}

	if (!found)
		LOG("Mesh not found");
}

const uint GameObject::GetNumMeshes() const
{
	uint num_meshes = 0;
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH)
			num_meshes++;
	return num_meshes;
}

const std::string & GameObject::GetName() const
{
	return name;
}

Mesh * GameObject::CreateMesh(const char *const name)
{
	Mesh* new_mesh;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Mesh %i", GetNumMeshes() + 1);
		new_mesh = new Mesh(new_name);
	}
	else
		new_mesh = new Mesh(name);

	components.push_back(new_mesh);

	return new_mesh;
}

void GameObject::DeleteMesh(const Mesh* to_delete)
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if (*it == to_delete)
		{
			delete *it;
			components.erase(it);
			break;
		}
}

const uint GameObject::GetNumComponents() const
{
	return components.size();
}
