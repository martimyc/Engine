#include <map>
#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "MathGeoLib\src\Geometry\Triangle.h"

#include "KDTreeGO.h"

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

GameObject::GameObject(const std::string& name, bool draw): name(name), draw(draw)
{
	local_transform = new Transform("Local Transform");
	world_transform = new Transform("World Transform");
	//bounds.sphere_bounding_box.SetNegativeInfinity();
	bounds.aabb_bounding_box.SetNegativeInfinity();
	//bounds.obb_bounding_box.SetNegativeInfinity();
	//bounds.obb_bounding_box.pos = vec(0, 0, 0);
	//bounds.original_obb_bounding_box.SetNegativeInfinity();
	//bounds.original_obb_bounding_box.pos = vec(0, 0, 0);
}

GameObject::GameObject(const GameObject & copy): name(copy.name), local_transform(new Transform("Local Transform")), world_transform(new Transform("World Transform")), draw(copy.draw), bounds(copy.bounds)
{
	local_transform->SetTransform(copy.local_transform->GetTransformMatrix());
	world_transform->SetTransform(copy.world_transform->GetTransformMatrix());

	for (std::vector<Component*>::const_iterator it = copy.components.begin(); it != copy.components.end(); ++it)
	{
		switch ((*it)->GetType())
		{
		case CT_MESH_FILTER:
			components.push_back(new MeshFilter(*((MeshFilter*)(*it))));
			break;
		case CT_APPLIED_MATERIAL:
			components.push_back(new AppliedMaterial(*((AppliedMaterial*)(*it))));
			break;
		case CT_CAMERA:
			break;
		}
	}

	for (std::vector<GameObject*>::const_iterator it = copy.childs.begin(); it != copy.childs.end(); ++it)
	{
		childs.push_back(new GameObject(*(*it)));
	}
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;
	components.clear();

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete *it;
	childs.clear();

	DELETE_PTR(local_transform);
	DELETE_PTR(world_transform);
}

bool GameObject::Update()
{
	bool ret = true;

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

void GameObject::SentToDraw(Camera* camera) const
{
	if (draw && GetMesh() != nullptr)
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

void GameObject::Inspector()
{
	ImGui::Begin("Inspector");
	//Transform
	if (local_transform->Inspector())
	{
		UpdateTransforms();
		App->scene_manager->UpdateKDT(this);
	}

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
	const Component* applied_material = GetAppliedMaterial();

	if (applied_material != nullptr)
		RemoveAppliedMaterial();
	
	AddComponent(new AppliedMaterial(new_material));	
}

void GameObject::ChangeMesh(Mesh * new_mesh)
{
	const Component* mesh_filter = GetMeshFilter();

	if(mesh_filter != nullptr)
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
		new_child = new GameObject(ptr);
	}
	else
		new_child = new GameObject(name);

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
	return local_transform->GetTransformMatrix();
}

math::float4x4 GameObject::GetWorldTransform() const
{
	return world_transform->GetTransformMatrix();
}

math::vec GameObject::GetMaxPos() const
{
	const Mesh* mesh = GetMesh();

	if (mesh == nullptr)
		return world_transform->GetTransformTranslation();

	return math::vec(mesh->GetMaxX(), mesh->GetMaxY(), mesh->GetMaxZ());
}

math::vec GameObject::GetMinPos() const
{
	const Mesh* mesh = GetMesh();

	if (mesh == nullptr)
		return world_transform->GetTransformTranslation();

	return math::vec(mesh->GetMinX(), mesh->GetMinY(), mesh->GetMinZ());
}

void GameObject::GetLocalPosX(int & x) const
{
	x = local_transform->GetTransformTranslation().x;
}

void GameObject::GetLocalPosY(int & y) const
{
	y = local_transform->GetTransformTranslation().y;
}

void GameObject::GetLocalPosZ(int & z) const
{
	z = local_transform->GetTransformTranslation().z;
}

math::vec GameObject::GetLocalPosition() const
{
	return local_transform->GetTransformTranslation();
}

void GameObject::GetWorldPosX(int & x) const
{
	x = local_transform->GetTransformTranslation().x;
}

void GameObject::GetWorldPosY(int & y) const
{
	y = local_transform->GetTransformTranslation().y;
}

void GameObject::GetWorldPosZ(int & z) const
{
	z = local_transform->GetTransformTranslation().z;
}

math::vec GameObject::GetWorldPosition() const
{
	return world_transform->GetTransformTranslation();
}

const math::vec& GameObject::GetLocalRotationEuler() const
{
	return local_transform->GetTransformRotationAngles();
}

math::vec GameObject::GetWorldRotationEuler() const
{
	return world_transform->GetTransformRotationAngles();
}

const math::Quat& GameObject::GetLocalRotationQuat() const
{
	return local_transform->GetTransformRotation();
}

math::Quat GameObject::GetWorldRotationQuat() const
{
	return world_transform->GetTransformRotation();
}

void GameObject::GetLocalScaleX(int & x) const
{
	x = local_transform->GetTransformScale().x;
}

void GameObject::GetLocalScaleY(int & y) const
{
	y = local_transform->GetTransformScale().y;
}

void GameObject::GetLocalScaleZ(int & z) const
{
	z = local_transform->GetTransformScale().z;
}

const math::vec&  GameObject::GetLocalScale(int & x, int & y, int & z) const
{
	return local_transform->GetTransformScale();
}

void GameObject::GetWorldScaleX(int & x) const
{
	x = world_transform->GetTransformTranslation().x;
}

void GameObject::GetWorldScaleY(int & y) const
{
	y = world_transform->GetTransformTranslation().y;
}

void GameObject::GetWorldScaleZ(int & z) const
{
	z = world_transform->GetTransformTranslation().z;
}

const math::vec&  GameObject::GetWorldScale(int & x, int & y, int & z) const
{
	return world_transform->GetTransformScale();;
}

const AABB * GameObject::GetAABB() const
{
	return &bounds.aabb_bounding_box;
}

bool GameObject::IsCamera() const
{
	return is_camera;
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
//	bounds.obb_bounding_box.Draw(1.0f, 1.0f, 0.0f, 1.0f);

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->DrawBoundingBoxes();
}

void GameObject::RemoveMeshFilter()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
		{
			delete *it;
			components.erase(it);
			return;
		}
}

void GameObject::RemoveAppliedMaterial()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
		{
			delete *it;
			components.erase(it);
			return;
		}
}

void GameObject::PickGameObject(const LineSegment* ray, float ray_distance) const
{
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

AABB* GameObject::UpdateAABBs(const GameObject* go)
{
	std::vector<AABB*> aabbs;

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it)->IsChild(go))
		{
			aabbs.push_back((*it)->UpdateAABBs(go));
		}
	}

	for (std::vector<AABB*>::const_iterator it = aabbs.begin(); it != aabbs.end(); ++it)
	{
		bounds.aabb_bounding_box.Enclose(*(*it));
	}
	
	/*bounds.aabb_bounding_box.minPoint = bounds.original_aabb_bb_points[0];
	bounds.aabb_bounding_box.maxPoint = bounds.original_aabb_bb_points[1];

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);
		(*it)->UpdateAABBs(go);
	}	
	bounds.aabb_bounding_box.TransformAsAABB(world_transform->GetTransformMatrix().Transposed());
	*/
	return &bounds.aabb_bounding_box;
}

bool GameObject::AddChildsToKDT(KDTreeGO & kdt) const
{
	bool ret = true;
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		if (kdt.AddGameObject(*it) == false)
			ret = false;
	return ret;
}

void GameObject::SetLocalTransform(const math::float4x4 & new_local_transform)
{
	math::float4x4 old_local = GetLocalTransform();
	math::float4x4 parent_world = world_transform->GetTransformMatrix() * old_local.Inverted();
	local_transform->SetTransform(new_local_transform);
	UpdateWorldTransform(parent_world);

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_CAMERA)
			((Camera*)(*it))->TransformCamera();

	UpdateBounds();
}

void GameObject::SetWorldTransform(const math::float4x4 & new_world_transform)
{
	math::float4x4 old_world = GetWorldTransform();
	math::float4x4 parent_world = old_world * GetLocalTransform().Inverted();
	world_transform->SetTransform(new_world_transform);
	local_transform->SetTransform(parent_world.Inverted() * new_world_transform);
	UpdateWorldTransform(world_transform->GetTransformMatrix());

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_CAMERA)
			((Camera*)(*it))->TransformCamera();

	UpdateBounds();
}

void GameObject::CreateBounds(const Mesh* mesh)
{
	bounds.original_aabb_bb_points[0] = bounds.aabb_bounding_box.minPoint = (math::vec(mesh->GetMinX(), mesh->GetMinY(), mesh->GetMinZ()));
	bounds.original_aabb_bb_points[1] = bounds.aabb_bounding_box.maxPoint = (math::vec(mesh->GetMaxX(), mesh->GetMaxY(), mesh->GetMaxZ()));

//	bounds.obb_bounding_box.SetFrom(bounds.aabb_bounding_box);
//	bounds.original_obb_bounding_box = bounds.obb_bounding_box;
}

void GameObject::UpdateBounds()
{
//	bounds.obb_bounding_box = bounds.original_obb_bounding_box;
//	bounds.obb_bounding_box.Transform(GetWorldTransform().Transposed());

//	App->scene_manager->UpdateAABBs(bounds.aabb_bounding_box);
//	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
//		bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);

	bounds.aabb_bounding_box.minPoint = bounds.original_aabb_bb_points[0];
	bounds.aabb_bounding_box.maxPoint = bounds.original_aabb_bb_points[1];
	bounds.aabb_bounding_box.TransformAsAABB(world_transform->GetTransformMatrix().Transposed());

	App->scene_manager->UpdateAABBs(this);

	GameObject* root = App->scene_manager->GetRoot();
	root->bounds.aabb_bounding_box.SetNegativeInfinity();
	for (std::vector<GameObject*>::const_iterator it = root->childs.begin(); it != root->childs.end(); ++it)
		root->bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);
}
bool GameObject::IsChild(const GameObject* go) const
{
	if (go == this)
		return true; 
	
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		if (go == (*it))
			return true;
	}

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		if ((*it)->IsChild(go))
			return true;
	}

	return false;
}
void GameObject::UpdateWorldTransform(const math::float4x4& parent_world_transform)
{
	world_transform->SetTransform(parent_world_transform * local_transform->GetTransformMatrix());
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->UpdateWorldTransform(world_transform->GetTransformMatrix());
	}
}

void GameObject::UpdateTransforms()
{
	math::float4x4 old_local = GetLocalTransform();
	math::float4x4 parent_world = world_transform->GetTransformMatrix() * old_local.Inverted();
	local_transform->Update();
	UpdateWorldTransform(parent_world);

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_CAMERA)
			((Camera*)(*it))->TransformCamera();

	UpdateBounds();
}

/*
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
			
			//vec obb_corner_points[8];
			//(*it)->bounds.obb_bounding_box.GetCornerPoints(obb_corner_points);
			//for (int i = 0; i < 8; i++)
			//	next_parent->bounds.obb_bounding_box.Enclose(obb_corner_points[i]);			
		}
		next_parent = next_parent->parent;
	}
}
*/