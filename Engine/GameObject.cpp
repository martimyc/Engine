#include <map>
#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3.h"
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
#include "Animator.h"
#include "Skeleton.h"
#include "Camera.h"

//modules
#include "Console.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "GameObject.h"

GameObject::GameObject(const std::string& name, bool draw) : name(name), draw(draw), parent(nullptr)
{
	local_transform = new Transform("Local Transform");
	world_transform = new Transform("World Transform");

	bounds.sphere_bounding_box.SetNegativeInfinity();
	bounds.sphere_bounding_box.r = 0.0f;
	bounds.original_sphere_bounding_box.SetNegativeInfinity();
	bounds.original_sphere_bounding_box.r = 0.0f;

	bounds.aabb_bounding_box.SetNegativeInfinity();
	bounds.original_aabb_bb_points[0] = vec(0.0f, 0.0f, 0.0f);
	bounds.original_aabb_bb_points[1] = vec(0.0f, 0.0f, 0.0f);

	bounds.obb_bounding_box.SetNegativeInfinity();
	bounds.original_obb_bounding_box.SetNegativeInfinity();
	bounds.obb_bounding_box.pos = bounds.original_obb_bounding_box.pos = math::vec::zero;
	bounds.obb_bounding_box.r = bounds.original_obb_bounding_box.r = math::vec::zero;
	bounds.obb_bounding_box.axis[0] = bounds.original_obb_bounding_box.axis[0] = math::vec(1, 0, 0);
	bounds.obb_bounding_box.axis[1] = bounds.original_obb_bounding_box.axis[1] = math::vec(0, 1, 0);
	bounds.obb_bounding_box.axis[2] = bounds.original_obb_bounding_box.axis[2] = math::vec(0, 0, 1);
}

GameObject::GameObject(const GameObject & copy) : name(copy.name), parent(copy.parent), local_transform(new Transform("Local Transform")), world_transform(new Transform("World Transform")), draw(copy.draw), bounds(copy.bounds)
{
	local_transform->SetTransform(copy.local_transform->GetTransformMatrix());
	world_transform->SetTransform(copy.world_transform->GetTransformMatrix());

	for (std::vector<Component*>::const_iterator it = copy.components.begin(); it != copy.components.end(); ++it)
	{
		switch ((*it)->GetType())
		{
		case CT_MESH_FILTER:
			components.push_back(new MeshFilter(*((MeshFilter*)(*it)), this));
			CreateBounds(((MeshFilter*)(*it))->GetMesh());
			break;
		case CT_APPLIED_MATERIAL:
			components.push_back(new AppliedMaterial(*((AppliedMaterial*)(*it)), this));
			break;
		case CT_CAMERA:
			break;
		case CT_ANIMATOR:
			Animator* animator_copy = (Animator*)(*it);
			components.push_back(new Animator(*animator_copy, this));
			break;
		}
	}

	for (std::vector<GameObject*>::const_iterator it = copy.childs.begin(); it != copy.childs.end(); ++it)
	{
		AddChild(new GameObject(*(*it)));
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
	
	case CT_ANIMATOR:

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
	App->BeginDockWindow("Inspector");
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

	if (ImGui::TreeNode("Debug"))
	{
		if (ImGui::TreeNode("Bounds"))
		{
			ImGui::Checkbox("Spheres", &draw_spheres);
			ImGui::Checkbox("AABBs", &draw_aabbs);
			ImGui::Checkbox("OBBs", &draw_obbs);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	App->EndDockWindow();
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
	
	AddComponent(new AppliedMaterial(new_material, this));	
}

void GameObject::ChangeMesh(Mesh * new_mesh)
{
	const Component* mesh_filter = GetMeshFilter();

	if(mesh_filter != nullptr)
		RemoveMeshFilter();

	AddComponent(new MeshFilter(new_mesh, this));
}

bool GameObject::Hirarchy(GameObject*& selected)
{
	bool ret = false;
	bool has_childs = childs.size() != 0;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected == this) ? ImGuiTreeNodeFlags_Selected : 0);

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
	if (child->parent != this)
		child->parent = this;
	child->world_transform->SetTransform(GetWorldTransform());
	child->SetWorldTransform(GetWorldTransform()); 
	childs.push_back(child);
}

GameObject* GameObject::CreateCamera()
{
	GameObject* new_child = CreateChild("New Camera");
	Camera* cam = new Camera("Camera", this);
	new_child->AddComponent(cam);
	new_child->is_camera = true;
	App->scene_manager->SetCameraFocused(cam);
	return new_child;
}

GameObject * GameObject::GetChild(const std::string & name) const
{
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		if (name == (*it)->name)
			return *it;

	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			return (*it)->GetChild(name);

	return nullptr;
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

	math::vec ret(mesh->GetMaxWorldVec(world_transform->GetTransformMatrix()));
	ret.x = std::round(ret.x * DECIMAL_NUM) / DECIMAL_NUM;
	ret.y = std::round(ret.y * DECIMAL_NUM) / DECIMAL_NUM;
	ret.z = std::round(ret.z * DECIMAL_NUM) / DECIMAL_NUM;

	return ret;
}

math::vec GameObject::GetMinPos() const
{
	const Mesh* mesh = GetMesh();

	if (mesh == nullptr)
		return world_transform->GetTransformTranslation();

	math::vec ret(mesh->GetMinWorldVec(world_transform->GetTransformMatrix()));
	ret.x = std::round(ret.x * DECIMAL_NUM) / DECIMAL_NUM;
	ret.y = std::round(ret.y * DECIMAL_NUM) / DECIMAL_NUM;
	ret.z = std::round(ret.z * DECIMAL_NUM) / DECIMAL_NUM;

	return ret;
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

const Sphere * GameObject::GetBoundingSphere() const
{
	return &bounds.sphere_bounding_box;
}

const AABB * GameObject::GetAABB() const
{
	return &bounds.aabb_bounding_box;
}

const OBB * GameObject::GetOBB() const
{
	return &bounds.obb_bounding_box;
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

AppliedMaterial * GameObject::GetAppliedMaterial() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_APPLIED_MATERIAL)
			return (AppliedMaterial*)*it;
	return nullptr;
}

MeshFilter * GameObject::GetMeshFilter() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_MESH_FILTER)
			return (MeshFilter*)*it;
	return nullptr;
}

Animator * GameObject::GetAnimator() const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_ANIMATOR)
			return (Animator*)*it;
	return nullptr;
}

void GameObject::DrawBoundingBoxes() const
{
	if (draw_spheres)
		bounds.sphere_bounding_box.Draw(0.0f, 1.0f, 0.0f, 1.0f); 
	if (draw_aabbs)
		bounds.aabb_bounding_box.Draw(1.0f, 1.0f, 0.0f, 1.0f);
	if (draw_obbs)
		bounds.obb_bounding_box.Draw(0.0f, 1.0f, 1.0f, 1.0f);

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
	//TODO both trough kdt and without it

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
		float shortest_distance = ray->Length();

		for (std::map<float, GameObject*>::iterator it = aabb_collisions.begin(); it != aabb_collisions.end(); ++it)
		{
			if ((*it).second->HasMeshFilter())
			{
				float distance = (*it).second->GetMesh()->RayCollision(ray);

				if (distance < shortest_distance)
				{
					shortest_distance = distance;
					go = (*it).second;
				}
			}
		}
		if (go != nullptr)
			App->scene_manager->SetFocused(go);
	}
}

bool GameObject::AddChildsToKDT(KDTreeGO & kdt) const
{
	bool ret = true;
	for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		if (kdt.AddGameObject(*it) == false)
			ret = false;
	return ret;
}

GameObject * GameObject::GetParent() const
{
	return parent;
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
}

void GameObject::CreateBounds(const Mesh* mesh)
{
	assert(!(mesh == nullptr));
	assert(mesh->IsLoaded());
	bounds.sphere_bounding_box.Enclose(mesh->GetMinWorldVec(world_transform->GetTransformMatrix()));
	bounds.sphere_bounding_box.Enclose(mesh->GetMaxWorldVec(world_transform->GetTransformMatrix()));
	bounds.original_sphere_bounding_box.Enclose(mesh->GetMinWorldVec(world_transform->GetTransformMatrix()));
	bounds.original_sphere_bounding_box.Enclose(mesh->GetMaxWorldVec(world_transform->GetTransformMatrix()));

	bounds.original_aabb_bb_points[0] = bounds.aabb_bounding_box.minPoint = (mesh->GetMinWorldVec(world_transform->GetTransformMatrix()));
	bounds.original_aabb_bb_points[1] = bounds.aabb_bounding_box.maxPoint = (mesh->GetMaxWorldVec(world_transform->GetTransformMatrix()));

	IncludeMeshInOBB(mesh);

	//Set center
	local_transform->SetTransformCenter(mesh->GetCenter());
}

void GameObject::UpdateBoundsParents()
{
	UpdateBoundsParent();

	if (parent != nullptr)
		parent->UpdateBoundsParents();
}

void GameObject::UpdateBoundsSelf()
{
	if (childs.size() == 0)
	{
		//Sphere
		bounds.sphere_bounding_box = bounds.original_sphere_bounding_box;
		bounds.sphere_bounding_box.Transform(world_transform->GetTransformMatrix().Transposed());

		//AABB
		bounds.aabb_bounding_box.minPoint = bounds.original_aabb_bb_points[0];
		bounds.aabb_bounding_box.maxPoint = bounds.original_aabb_bb_points[1];
		bounds.aabb_bounding_box.TransformAsAABB(world_transform->GetTransformMatrix().Transposed());

		//OBB
		bounds.obb_bounding_box = bounds.original_obb_bounding_box;
		bounds.obb_bounding_box.Transform(world_transform->GetTransformMatrix().Transposed());

		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			//Sphere
			bounds.sphere_bounding_box.Enclose((*it)->bounds.sphere_bounding_box);

			//AABB
			bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);

			//OBB
			math::vec obb_points[8];
			(*it)->bounds.obb_bounding_box.GetCornerPoints(obb_points);
			for (int i = 0; i < 8; ++i)
				bounds.obb_bounding_box.Enclose(obb_points[i]);
		}
	}
}

void GameObject::UpdateBoundsParent()
{
	if (parent != nullptr)
	{
		//Sphere
		parent->bounds.sphere_bounding_box = parent->bounds.original_sphere_bounding_box;
		parent->bounds.sphere_bounding_box.Transform(parent->GetWorldTransform().Transposed());

		//AABB
		parent->bounds.aabb_bounding_box.SetNegativeInfinity();
		parent->bounds.aabb_bounding_box.minPoint = parent->bounds.original_aabb_bb_points[0];
		parent->bounds.aabb_bounding_box.maxPoint = parent->bounds.original_aabb_bb_points[1];
		parent->bounds.aabb_bounding_box.TransformAsAABB(parent->GetWorldTransform().Transposed());

		//OBB
		parent->bounds.obb_bounding_box = parent->bounds.original_obb_bounding_box;
		parent->bounds.obb_bounding_box.Transform(parent->GetWorldTransform().Transposed());

		for (std::vector<GameObject*>::iterator it = parent->childs.begin(); it != parent->childs.end(); ++it)
		{
			//Sphere
			/*if ((*it)->bounds.sphere_bounding_box.r > EPSILON)
				parent->bounds.sphere_bounding_box.Enclose((*it)->bounds.sphere_bounding_box);
			else
				parent->bounds.sphere_bounding_box.Enclose((*it)->bounds.sphere_bounding_box.pos);*/

			//AABB
			parent->bounds.aabb_bounding_box.Enclose((*it)->bounds.aabb_bounding_box);

			//OBB
			math::vec obb_points[8];
			(*it)->bounds.obb_bounding_box.GetCornerPoints(obb_points);
			for (int i = 0; i < 8; ++i)
				parent->bounds.obb_bounding_box.Enclose(obb_points[i]);
		}
	}
}

void GameObject::IncludeMeshInOBB(const Mesh * mesh)
{
	bounds.obb_bounding_box.pos = mesh->GetWorldCenter(world_transform->GetTransformMatrix());
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMaxXVertex(world_transform->GetTransformMatrix()));
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMinXVertex(world_transform->GetTransformMatrix()));
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMaxYVertex(world_transform->GetTransformMatrix()));
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMinYVertex(world_transform->GetTransformMatrix()));
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMaxZVertex(world_transform->GetTransformMatrix()));
	bounds.obb_bounding_box.Enclose(mesh->GetWorldMinZVertex(world_transform->GetTransformMatrix()));

	bounds.original_obb_bounding_box.pos = mesh->GetWorldCenter(world_transform->GetTransformMatrix());
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMaxXVertex(world_transform->GetTransformMatrix()));
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMinXVertex(world_transform->GetTransformMatrix()));
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMaxYVertex(world_transform->GetTransformMatrix()));
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMinYVertex(world_transform->GetTransformMatrix()));
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMaxZVertex(world_transform->GetTransformMatrix()));
	bounds.original_obb_bounding_box.Enclose(mesh->GetWorldMinZVertex(world_transform->GetTransformMatrix()));
}

void GameObject::UpdateWorldTransform(const math::float4x4& parent_world_transform)
{
	world_transform->SetTransform(local_transform->GetTransformMatrix() * parent_world_transform);

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->UpdateWorldTransform(world_transform->GetTransformMatrix());

	UpdateBoundsSelf();
	UpdateBoundsParents();
}

void GameObject::UpdateTransforms()
{
	local_transform->Update();							//new local transform
	UpdateWorldTransform(parent->GetWorldTransform());	//new world transform

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == CT_CAMERA)
			((Camera*)(*it))->TransformCamera();
}