#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Math\float3x3.h"

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
	if (transform->Update())
		UpdateBounds();


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

		CreateBounds(((MeshFilter*)component)->GetMesh());

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
	float4x4 transf_matrix = transform->GetTransformMatrix();
	const GameObject* temp_parent = parent;
	while (temp_parent != nullptr)
	{
		transf_matrix = transf_matrix * (temp_parent->transform->GetTransformMatrix());
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

void GameObject::GetWorldPosition(int & x, int & y, int & z) const
{
	const GameObject* next_parent = parent;
	math::float3 pos = transform->GetTransformTranslation();
	while (next_parent)
	{
		pos += next_parent->transform->GetTransformTranslation();
		next_parent = next_parent->parent;
	}

	x = pos.x;
	y = pos.y;
	z = pos.z;
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
	uint num_vertices = mesh->GetNumVertices();
	const GLfloat* all_vertices = mesh->GetVertices();

	math::vec* vec_vertices = new math::vec[num_vertices];
	memcpy(vec_vertices, all_vertices, num_vertices * sizeof(GLfloat) * 3);

	AABB_bounding_box.Enclose(vec_vertices, num_vertices);
	original_AABB_bounding_box = AABB_bounding_box;

	DELETE_ARRAY(vec_vertices);
}

void GameObject::UpdateBounds()
{
	const GameObject* next_parent = parent;
	math::float4x4 matrix_transformation = transform->GetTransformMatrix();
	while (next_parent)
	{
		matrix_transformation = matrix_transformation * next_parent->transform->GetTransformMatrix();
		next_parent = next_parent->parent;
	}

	AABB_bounding_box = original_AABB_bounding_box;
	AABB_bounding_box.TransformAsAABB(matrix_transformation.Transposed());

	//Update Childs
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->UpdateBounds();
}

void GameObject::DrawAABBBoundingBox() const
{
	math::float3 aabb_vertices[8];

	AABB_bounding_box.GetCornerPoints(aabb_vertices);

	glLineWidth(3.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glBegin(GL_LINES);

	glVertex3f(aabb_vertices[0].x, aabb_vertices[0].y, aabb_vertices[0].z);
	glVertex3f(aabb_vertices[1].x, aabb_vertices[1].y, aabb_vertices[1].z);

	glVertex3f(aabb_vertices[0].x, aabb_vertices[0].y, aabb_vertices[0].z);
	glVertex3f(aabb_vertices[2].x, aabb_vertices[2].y, aabb_vertices[2].z);

	glVertex3f(aabb_vertices[0].x, aabb_vertices[0].y, aabb_vertices[0].z);
	glVertex3f(aabb_vertices[4].x, aabb_vertices[4].y, aabb_vertices[4].z);

	glVertex3f(aabb_vertices[1].x, aabb_vertices[1].y, aabb_vertices[1].z);
	glVertex3f(aabb_vertices[3].x, aabb_vertices[3].y, aabb_vertices[3].z);

	glVertex3f(aabb_vertices[1].x, aabb_vertices[1].y, aabb_vertices[1].z);
	glVertex3f(aabb_vertices[5].x, aabb_vertices[5].y, aabb_vertices[5].z);

	glVertex3f(aabb_vertices[2].x, aabb_vertices[2].y, aabb_vertices[2].z);
	glVertex3f(aabb_vertices[3].x, aabb_vertices[3].y, aabb_vertices[3].z);

	glVertex3f(aabb_vertices[2].x, aabb_vertices[2].y, aabb_vertices[2].z);
	glVertex3f(aabb_vertices[6].x, aabb_vertices[6].y, aabb_vertices[6].z);

	glVertex3f(aabb_vertices[3].x, aabb_vertices[3].y, aabb_vertices[3].z);
	glVertex3f(aabb_vertices[7].x, aabb_vertices[7].y, aabb_vertices[7].z);

	glVertex3f(aabb_vertices[5].x, aabb_vertices[5].y, aabb_vertices[5].z);
	glVertex3f(aabb_vertices[4].x, aabb_vertices[4].y, aabb_vertices[4].z);

	glVertex3f(aabb_vertices[5].x, aabb_vertices[5].y, aabb_vertices[5].z);
	glVertex3f(aabb_vertices[7].x, aabb_vertices[7].y, aabb_vertices[7].z);

	glVertex3f(aabb_vertices[4].x, aabb_vertices[4].y, aabb_vertices[4].z);
	glVertex3f(aabb_vertices[6].x, aabb_vertices[6].y, aabb_vertices[6].z);

	glVertex3f(aabb_vertices[6].x, aabb_vertices[6].y, aabb_vertices[6].z);
	glVertex3f(aabb_vertices[7].x, aabb_vertices[7].y, aabb_vertices[7].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}