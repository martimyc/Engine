#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"

//components
#include "MeshFilter.h"
#include "AppliedMaterial.h"
#include "Transform.h"
#include "Texture.h"

//modules
#include "Console.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"

GameObject::GameObject(const GameObject* const parent, const char* name): parent(parent), name(name)
{}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;
	components.clear();

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete *it;
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

void GameObject::SentToDraw() const
{
	if (draw)
	{
		App->renderer_3d->DrawGameObject(this);
		/*if (components.size() > 0)
		{
			std::vector<Component*>::const_iterator transform_it;
			for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
			if ((*it)->Enabled())
			if ((*it)->GetType() == CT_TRANSFORMATION)
			{
			transform_it = it;
			((Transform*)*it)->TranslateRotateScalate();
			}

			//((Transform*)*transform_it)->ResetTranslateRotateScalate();
		}*/
	}

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->SentToDraw();
}

void GameObject::AddComponent(Component * component)
{
	bool add = true;

	switch (component->GetType())
	{
	case CT_MESH_FILTER:
		if (HasMeshFilter())
			CreateChild(component);
		else
			draw = true;
		break;
	case CT_APPLIED_MATERIAL:
		if (HasAppliedMaterial())
		{
			ChangeMaterial((Material*)component);
			add = false;
		}
		break;
	default:
		break;
	}
	
	if(add)
		components.push_back(component);
}

void GameObject::Inspector() const
{
	if (components.size() > 0)
	{
		ImGui::Begin("Inspector");

		for (int i = 0; i < components.size(); i++)
			components[i]->Inspector();

		ImGui::End();
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
		if ((*it)->GetType() == CT_TRANSFORM)
			return ((Transform*)*it)->GetTransformMatrix();
	}
	return nullptr;
}

void GameObject::ChangeMaterial(Material * new_material)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			RemoveAppliedMaterial();
	
	AddComponent(new AppliedMaterial(new_material));	
}

bool GameObject::Hirarchy(GameObject*& selected)
{
	bool ret = false;
	bool has_childs = childs.size() != 0;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected == this) ? ImGuiTreeNodeFlags_Selected : 0);

	if (!has_childs)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
		node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool node_open = ImGui::TreeNodeEx(this, node_flags, name.c_str()); //TODO try with TreeNode*

	if (ImGui::IsItemClicked() && ret == false)
	{
		selected = this;
		ret = true;
	}

	if (node_open)
	{
		for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			if ((*it)->Hirarchy(selected))
				ret = true;

		if (has_childs)
			ImGui::TreePop();
	}

	if (selected == this)
		Inspector();

	return ret;
}

GameObject * GameObject::CreateChild()
{
	char name[255];
	sprintf(name, "Child %i", childs.size());
	GameObject* new_child = new GameObject(this, name);
	AddChild(new_child);
	return new_child;
}

GameObject * GameObject::CreateChild(Component * component)
{
	GameObject* new_child = CreateChild();
	new_child->AddComponent(component);
	return new_child;
}

void GameObject::AddChild(GameObject* child)
{
	childs.push_back(child);
}

const unsigned int GameObject::GetNumComponents() const
{
	return components.size();
}

const std::string & GameObject::GetName() const
{
	return name;
}

bool GameObject::HasMeshFilter() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
			return true;
	return false;
}

bool GameObject::HasAppliedMaterial() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			return true;
	return false;
}

const AppliedMaterial * GameObject::GetAppliedMaterial() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			return (AppliedMaterial*)*it;
	return nullptr;
}

const MeshFilter * GameObject::GetMeshFilter() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
			return (MeshFilter*)*it;
	return nullptr;
}

void GameObject::RemoveMeshFilter()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
		{
			delete *it;
			components.erase(it);
			break;
		}
}

void GameObject::RemoveAppliedMaterial()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
		{
			delete *it;
			components.erase(it);
			break;
		}
}

Transform * GameObject::CreateTransformation(const char* const name)
{
	Transform* transformation;
	if (!name)
		transformation = new Transform();
	else
		transformation = new Transform(name);

	components.push_back(transformation);

	return transformation;
}

