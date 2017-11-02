#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"

#include "Mesh.h"
//components
#include "MeshFilter.h"
#include "AppliedMaterial.h"
#include "Transform.h"
#include "Texture.h"
#include "Component.h"

//modules
#include "Console.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"

GameObject::GameObject(const GameObject* const parent, const char* name): parent(parent), name(name)
{
	transform = new Transform("Transform");
	sphere_bounding_box.SetNegativeInfinity();
	AABB_bounding_box.SetNegativeInfinity();
	//AABB_bounding_box = AABB(vec(4, 4, 4), vec(8, 8, 8));
	OBB_bounding_box.SetNegativeInfinity();
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;
	components.clear();

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete *it;
	childs.clear();

	DELETE_PTR(transform);
}

bool GameObject::Update()
{
	bool ret = true;

	//Update Transformation
	transform->Update();

	//Update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		ret = (*it)->Update();
		if (ret == false)
		{
			LOG("Error updating component -%s- in game object -%s-", (*it)->GetName().c_str(), name.c_str());
			break;
		}
	}

	//Update childs
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->Update();

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

		CreateBounds(((MeshFilter*)component)->GetMesh());//TODO:
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

	if (add)
		components.push_back(component);
}

void GameObject::Inspector() const
{
	ImGui::Begin("Inspector");
	//Transform
	transform->Inspector();

	//Other components
	if (components.size() > 0)
	{
		for (int i = 0; i < components.size(); i++)
			components[i]->Inspector();
	}
	ImGui::End();
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
	float4x4 transf_matrix = *transform->GetTransformMatrix();
	const GameObject* temp_parent = parent;
	while (temp_parent != nullptr)
	{
		transf_matrix = transf_matrix * (*temp_parent->transform->GetTransformMatrix());
		temp_parent = temp_parent->parent;
	}

	return &transf_matrix.At(0, 0);
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

	return ret;
}

GameObject * GameObject::CreateChild(const char* const name)
{
	GameObject* new_child = nullptr;

	if (name == nullptr)
	{
		char ptr[255];
		sprintf(ptr, "Child %i", childs.size());
		new_child = new GameObject(this, ptr);
	}
	else
		new_child = new GameObject(this, name);

	AddChild(new_child);

	return new_child;
}

GameObject * GameObject::CreateChild(Component * component, const char* const name)
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

void GameObject::DrawBoundingBoxes() const
{
	DrawAABBBoundingBox();

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

void GameObject::CreateBounds(const Mesh* mesh)
{
	uint id, num_vertices;
	float* all_vertices = nullptr;
	mesh->GetVertices(id, num_vertices, all_vertices);
	vec* vertices_vec = new vec(all_vertices);
	/*for (int i = 0; i < num_vertices; i++)
	{
		vertices_vec[i].x = all_vertices[i * 3];
		vertices_vec[i].y = all_vertices[i * 3 + 1];
		vertices_vec[i].z = all_vertices[i * 3 + 2];
	}*/

	AABB_bounding_box.Enclose(vertices_vec, num_vertices);
	delete vertices_vec;
}

void GameObject::DrawAABBBoundingBox() const
{
	math::float3 aabb_verteices[8];

	AABB_bounding_box.GetCornerPoints(aabb_verteices);

	glLineWidth(3.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glBegin(GL_LINES);

	glVertex3f(aabb_verteices[0].x, aabb_verteices[0].y, aabb_verteices[0].z);
	glVertex3f(aabb_verteices[1].x, aabb_verteices[1].y, aabb_verteices[1].z);

	glVertex3f(aabb_verteices[0].x, aabb_verteices[0].y, aabb_verteices[0].z);
	glVertex3f(aabb_verteices[2].x, aabb_verteices[2].y, aabb_verteices[2].z);

	glVertex3f(aabb_verteices[0].x, aabb_verteices[0].y, aabb_verteices[0].z);
	glVertex3f(aabb_verteices[4].x, aabb_verteices[4].y, aabb_verteices[4].z);

	glVertex3f(aabb_verteices[1].x, aabb_verteices[1].y, aabb_verteices[1].z);
	glVertex3f(aabb_verteices[3].x, aabb_verteices[3].y, aabb_verteices[3].z);

	glVertex3f(aabb_verteices[1].x, aabb_verteices[1].y, aabb_verteices[1].z);
	glVertex3f(aabb_verteices[5].x, aabb_verteices[5].y, aabb_verteices[5].z);

	glVertex3f(aabb_verteices[2].x, aabb_verteices[2].y, aabb_verteices[2].z);
	glVertex3f(aabb_verteices[3].x, aabb_verteices[3].y, aabb_verteices[3].z);

	glVertex3f(aabb_verteices[2].x, aabb_verteices[2].y, aabb_verteices[2].z);
	glVertex3f(aabb_verteices[6].x, aabb_verteices[6].y, aabb_verteices[6].z);

	glVertex3f(aabb_verteices[3].x, aabb_verteices[3].y, aabb_verteices[3].z);
	glVertex3f(aabb_verteices[7].x, aabb_verteices[7].y, aabb_verteices[7].z);

	glVertex3f(aabb_verteices[5].x, aabb_verteices[5].y, aabb_verteices[5].z);
	glVertex3f(aabb_verteices[4].x, aabb_verteices[4].y, aabb_verteices[4].z);

	glVertex3f(aabb_verteices[5].x, aabb_verteices[5].y, aabb_verteices[5].z);
	glVertex3f(aabb_verteices[7].x, aabb_verteices[7].y, aabb_verteices[7].z);

	glVertex3f(aabb_verteices[4].x, aabb_verteices[4].y, aabb_verteices[4].z);
	glVertex3f(aabb_verteices[6].x, aabb_verteices[6].y, aabb_verteices[6].z);

	glVertex3f(aabb_verteices[6].x, aabb_verteices[6].y, aabb_verteices[6].z);
	glVertex3f(aabb_verteices[7].x, aabb_verteices[7].y, aabb_verteices[7].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}