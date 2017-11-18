#include <map>
#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"

#include "MathGeoLib\src\Geometry\Triangle.h"

//components
#include "Component.h"
#include "MeshFilter.h"
#include "AppliedMaterial.h"
#include "Transform.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

//modules
#include "Console.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "GameObject.h"

GameObject::GameObject(GameObject* const parent, const std::string& name): parent(parent), name(name)
{
	transform = new Transform("Transform");
	bounds.sphere_bounding_box.SetNegativeInfinity();
	bounds.aabb_bounding_box.SetNegativeInfinity();
	bounds.obb_bounding_box.SetNegativeInfinity();
	bounds.obb_bounding_box.pos = vec(0, 0, 0);
	bounds.original_obb_bounding_box.SetNegativeInfinity();
	bounds.original_obb_bounding_box.pos = vec(0, 0, 0);
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
	{
		transformed = true;
		UpdateBounds();
	}

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

	transformed = false;

	//Update childs
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->Update();

	return ret;
}

void GameObject::SentToDraw(Camera* camera) const
{
	if (draw)
	{
		if (camera != nullptr)
		{
			if (camera->IsFrustumActive())
			{
				if (camera->FrustumCulling(this))
					App->renderer_3d->DrawGameObject(this);
			}
		}
		else
			App->renderer_3d->DrawGameObject(this);
	}

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->SentToDraw(camera);
}

void GameObject::AddComponent(Component * component)
{
	bool add = true;

	component->SetGameObject(this);

	switch (component->GetType())
	{
	case CT_MESH_FILTER:
		if (HasMeshFilter())
			CreateChild(component);
		else
			draw = true;

		CreateBounds(((MeshFilter*)component)->GetMesh());
		UpdateBoundsUpwards();

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
	
	component->Start();

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

void GameObject::Delete(GameObject * to_delete)
{
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		if (*it == to_delete)
		{
			delete *it;
			childs.erase(it);
			return;
		}
}

void GameObject::ChangeMaterial(Material * new_material)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			RemoveAppliedMaterial();
	
	AddComponent(new AppliedMaterial(new_material));	
}

void GameObject::ChangeMesh(Mesh * new_mesh)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
			RemoveMeshFilter();

	AddComponent(new MeshFilter(new_mesh));
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
	GameObject* new_child = CreateChild(name);
	new_child->AddComponent(component);
	return new_child;
}

void GameObject::AddChild(GameObject* child)
{
	childs.push_back(child);
	
	if(child->parent != this)
		child->parent = this;
}

GameObject* GameObject::CreateCamera()
{
	GameObject* new_child = CreateChild("New Camera");
	Camera* cam = new Camera("Camera");
	new_child->AddComponent(cam);
	new_child->is_camera = true;
	App->scene_manager->SetCameraFocused(cam);
	return new_child;
}

const unsigned int GameObject::GetNumComponents() const
{
	return components.size();
}

const std::string & GameObject::GetName() const
{
	return name;
}

const Mesh * GameObject::GetMesh() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
			return ((MeshFilter*)(*it))->GetMesh();
	return nullptr;
}

const Material * GameObject::GetMaterial() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			return ((AppliedMaterial*)(*it))->GetMaterial();
	return nullptr;
}

math::float4x4 GameObject::GetLocalTransform() const
{
	return transform->GetTransformMatrix();
}

const float * GameObject::GetLocalGLTransform() const
{
	math::float4x4 ret = transform->GetTransformMatrix();
	return &ret.At(0, 0);
}

math::float4x4 GameObject::GetWorldTransform() const
{
	const GameObject* next_parent = parent;
	math::float4x4 ret = transform->GetTransformMatrix();
	while (next_parent != nullptr)
	{
		ret = ret * next_parent->transform->GetTransformMatrix();
		next_parent = next_parent->parent;
	}
	return ret;
}

const float * GameObject::GetWorldGLTransform() const
{
	const GameObject* next_parent = parent;
	math::float4x4 ret = transform->GetTransformMatrix();
	while (next_parent != nullptr)
	{
		ret = ret * next_parent->transform->GetTransformMatrix();
		next_parent = next_parent->parent;
	}

	return &ret.At(0, 0);
}

void GameObject::GetLocalPosX(int & x) const
{
	x = transform->GetTransformTranslation().x;
}

void GameObject::GetLocalPosY(int & y) const
{
	y = transform->GetTransformTranslation().y;
}

void GameObject::GetLocalPosZ(int & z) const
{
	z = transform->GetTransformTranslation().z;
}

math::vec GameObject::GetLocalPosition() const
{
	return transform->GetTransformTranslation();
}

void GameObject::GetWorldPosX(int & x) const
{
	const GameObject* next_parent = parent;
	x = transform->GetTransformTranslation().x;
	while (next_parent)
	{
		x += next_parent->transform->GetTransformTranslation().x;
		next_parent = next_parent->parent;
	}
}

void GameObject::GetWorldPosY(int & y) const
{
	const GameObject* next_parent = parent;
	y = transform->GetTransformTranslation().y;
	while (next_parent)
	{
		y += next_parent->transform->GetTransformTranslation().y;
		next_parent = next_parent->parent;
	}
}

void GameObject::GetWorldPosZ(int & z) const
{
	const GameObject* next_parent = parent;
	z = transform->GetTransformTranslation().z;
	while (next_parent)
	{
		z += next_parent->transform->GetTransformTranslation().z;
		next_parent = next_parent->parent;
	}
}

math::vec GameObject::GetWorldPosition() const
{
	const GameObject* next_parent = parent;
	math::float3 pos = transform->GetTransformTranslation();
	while (next_parent)
	{
		pos += next_parent->transform->GetTransformTranslation();
		next_parent = next_parent->parent;
	}
	return pos;
}

const math::vec& GameObject::GetLocalRotationEuler() const
{
	return transform->GetTransformRotationAngles();
}

math::vec GameObject::GetWorldRotationEuler() const
{
	const GameObject* next_parent = parent;
	math::vec rotation_euler = transform->GetTransformRotationAngles();
	while (next_parent)
	{
		rotation_euler += next_parent->transform->GetTransformRotationAngles();
		next_parent = next_parent->parent;
	}
	return rotation_euler;
}

const math::Quat& GameObject::GetLocalRotationQuat() const
{
	return transform->GetTransformRotation();
}

math::Quat GameObject::GetWorldRotationQuat() const
{
	const GameObject* next_parent = parent;
	math::vec rotation_euler = transform->GetTransformRotationAngles();
	while (next_parent)
	{
		rotation_euler += next_parent->transform->GetTransformRotationAngles();
		next_parent = next_parent->parent;
	}
	return transform->TransformEuler2Quat(rotation_euler);
}

void GameObject::GetLocalScaleX(int & x) const
{
	x = transform->GetTransformScale().x;
}

void GameObject::GetLocalScaleY(int & y) const
{
	y = transform->GetTransformScale().y;
}

void GameObject::GetLocalScaleZ(int & z) const
{
	z = transform->GetTransformScale().z;
}

const math::vec&  GameObject::GetLocalScale(int & x, int & y, int & z) const
{
	return transform->GetTransformScale();
}

void GameObject::GetWorldScaleX(int & x) const
{
	const GameObject* next_parent = parent;
	x = transform->GetTransformTranslation().x;
	while (next_parent)
	{
		x *= next_parent->transform->GetTransformScale().x;
		next_parent = next_parent->parent;
	}
}

void GameObject::GetWorldScaleY(int & y) const
{
	const GameObject* next_parent = parent;
	y = transform->GetTransformTranslation().y;
	while (next_parent)
	{
		y *= next_parent->transform->GetTransformScale().y;
		next_parent = next_parent->parent;
	}
}

void GameObject::GetWorldScaleZ(int & z) const
{
	const GameObject* next_parent = parent;
	z = transform->GetTransformTranslation().z;
	while (next_parent)
	{
		z *= next_parent->transform->GetTransformScale().z;
		next_parent = next_parent->parent;
	}
}

const math::vec&  GameObject::GetWorldScale(int & x, int & y, int & z) const
{
	const GameObject* next_parent = parent;
	math::float3 scale = transform->GetTransformTranslation();
	while (next_parent)
	{
		scale.Mul(next_parent->transform->GetTransformScale());
		next_parent = next_parent->parent;
	}
	return scale;
}

const AABB * GameObject::GetAABB() const
{
	return &bounds.aabb_bounding_box;
}

bool GameObject::IsCamera() const
{
	return is_camera;
}

void GameObject::SetTransform(const math::float4x4& new_transform)
{
	transform->SetTransform(new_transform);
}

void GameObject::SetDraw(bool draw_)
{
	draw = draw_;
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
	bounds.aabb_bounding_box.Draw(0.7f, 0.7f, 0.0f, 1.0f);
	bounds.obb_bounding_box.Draw(1.0f, 1.0f, 0.0f, 1.0f);
	const GameObject* next_parent = parent;
	while (next_parent)
	{
		next_parent->bounds.aabb_bounding_box.Draw(0.7f, 0.7f, 0.0f, 1.0f);
		next_parent->bounds.obb_bounding_box.Draw(1.0f, 1.0f, 0.0f, 1.0f);
		next_parent = next_parent->parent;
	}
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

void GameObject::PickGameObject(const LineSegment* ray, float ray_distance) const
{
	//if(this->HasMeshFilter())

	std::map<float, GameObject*> aabb_collisions;
	LineSegment ray_local_space(*ray);

	//Check AABBs
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		ray_local_space.Transform((*it)->GetLocalTransform());

		if (ray_local_space.Intersects((*it)->bounds.aabb_bounding_box))
		{
			float d = (*it)->bounds.aabb_bounding_box.Distance(ray->GetPoint(0));
			aabb_collisions.insert(std::pair<float, GameObject*>(d, *it));
		}

		(*it)->PickGameObject(ray, ray_distance);
	}

	if (aabb_collisions.size() == 0)
		App->scene_manager->SetFocused(App->scene_manager->GetRoot());
	else
	{
		//Check triangles
		GameObject* go = nullptr;
		math::Triangle tri;
		math::Triangle triangle_to_test;

		for (std::map<float, GameObject*>::iterator it = aabb_collisions.begin(); it != aabb_collisions.end(); ++it)
		{
			if ((*it).second->HasMeshFilter())
			{
				if ((*it).second->GetMesh()->RayCollisionKDT(ray, triangle_to_test))
				{
					if (go != nullptr)
					{
						if (triangle_to_test.Distance(ray->a) < tri.Distance(ray->a))
						{
							tri = triangle_to_test;
							go = (*it).second;
						}
					}
					else
					{
						tri = triangle_to_test;
						go = (*it).second;
					}
				}
			}
		}
		if (go != nullptr)
			App->scene_manager->SetFocused(go);
	}
}

void GameObject::CreateBounds(const Mesh* mesh)
{
	uint num_vertices = mesh->GetNumVertices();
	const GLfloat* all_vertices = mesh->GetVertices();

	math::vec* vec_vertices = new math::vec[num_vertices];
	memcpy(vec_vertices, all_vertices, num_vertices * sizeof(GLfloat) * 3);

	bounds.aabb_bounding_box.Enclose(vec_vertices, num_vertices);
	bounds.original_aabb_bb_points[0] = bounds.aabb_bounding_box.minPoint;
	bounds.original_aabb_bb_points[1] = bounds.aabb_bounding_box.maxPoint;

	bounds.obb_bounding_box.SetFrom(bounds.aabb_bounding_box);
	bounds.original_obb_bounding_box = bounds.obb_bounding_box;

	DELETE_ARRAY(vec_vertices);
}

void GameObject::UpdateBounds()
{
	bounds.aabb_bounding_box.minPoint = bounds.original_aabb_bb_points[0];
	bounds.aabb_bounding_box.maxPoint = bounds.original_aabb_bb_points[1];
	bounds.aabb_bounding_box.TransformAsAABB(GetWorldTransform().Transposed());

	bounds.obb_bounding_box = bounds.original_obb_bounding_box;
	bounds.obb_bounding_box.Transform(GetWorldTransform().Transposed());

	//Update Childs
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->UpdateBounds();

	UpdateBoundsUpwards();
}

void GameObject::UpdateBoundsUpwards()
{
	if (parent == nullptr)
	{
		LOG("Cant update bounds upard, no parent");
		return;
	}

	GameObject* next_parent = parent;
	while (next_parent->parent != nullptr)
	{
		next_parent->bounds.aabb_bounding_box.SetNegativeInfinity();
		//next_parent->bounds.obb_bounding_box.SetNegativeInfinity();
		for (std::vector<GameObject*>::const_iterator it = next_parent->childs.begin(); it != next_parent->childs.end(); ++it)
		{
			next_parent->bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);
			/*
			vec obb_corner_points[8];
			(*it)->bounds.obb_bounding_box.GetCornerPoints(obb_corner_points);
			for (int i = 0; i < 8; i++)
				next_parent->bounds.obb_bounding_box.Enclose(obb_corner_points[i]);
			*/
		}
		next_parent = next_parent->parent;
	}
}