#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Mesh.h"
#include "Console.h"
#include "Material.h"
#include "Transformation.h"
#include "Texture.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"

GameObject::GameObject(const TreeNode* const node, const char* name): tree_node(node), name(name)
{}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete (*it);
	components.clear();
}

bool GameObject::Update()
{
	bool ret = true;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		ret = (*it)->Update();
		if (ret == false)
		{
			LOG("Error updating component -%s- in game object -%s-", (*it)->GetName().c_str(), name.c_str());
			break;
		}
	}

	return ret;
}

void GameObject::Draw() const
{
	if (components.size() > 0)
	{
		/*std::vector<Component*>::const_iterator transform_it;
		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
			if ((*it)->Enabled())
				if ((*it)->GetType() == CT_TRANSFORMATION)
				{
					transform_it = it;
					((Transform*)*it)->TranslateRotateScalate();
				}*/
	
		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
			if ((*it)->Enabled())
				if((*it)->GetType() == CT_MESH)
					App->renderer_3d->DrawMesh((Mesh*)(*it));

		//((Transform*)*transform_it)->ResetTranslateRotateScalate();
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

		//TODO rot and pos plus component management config at least

		for (int i = 0; i < components.size(); i++)
			components[i]->Inspector(i);

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

const float * const GameObject::GetTransformationMatrix()const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == CT_TRANSFORMATION)
			return ((Transform*)*it)->GetTransformMatrix();
	}
	return nullptr;
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
		new_mesh = new Mesh(new_name, this);
	}
	else
		new_mesh = new Mesh(name, this);

	components.push_back(new_mesh);

	return new_mesh;
}

Transform * GameObject::CreateTransformation(const char* const name)
{
	Transform* transformation;
	if (!name)
		transformation = new Transform("Transformation", this);
	else
		transformation = new Transform(name, this);

	components.push_back(transformation);

	return transformation;
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
