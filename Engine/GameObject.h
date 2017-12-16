#ifndef GAME_OBJECT
#define GAME_OBJECT

#include <vector>
#include <string>
#include <map>
#include "glew\include\GL\glew.h"
#include "Bounds.h"

class Component;
class Transform;
class AppliedMaterial;
class MeshFilter;
class Material;
class Mesh;
class Camera;
class Animator;
class Skeleton;
class KDTreeGO;

namespace math
{
	class LineSegment;
}

class GameObject
{
private:
	char name[128];
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	GameObject* parent = nullptr;
	Transform* local_transform = nullptr;
	Transform* world_transform = nullptr;
	Bounds bounds;
	bool is_camera = false;
	bool draw = false;

	bool draw_spheres = false;
	bool draw_aabbs = false;
	bool draw_obbs = false;

public:
	GameObject(const char name_[128], bool draw = true);
	GameObject(const GameObject& copy);
	~GameObject();

private:
	//Bounding Boxes
	void CreateBounds(const Mesh* mesh);
	void UpdateBoundsParents();

	void UpdateBoundsSelf();
	void UpdateBoundsParent();
	void IncludeMeshInOBB(const Mesh* mesh);
	void UpdateWorldTransform(const math::float4x4& parent_world_transform);
	void UpdateTransforms();

	bool RayBoundsCollision(const LineSegment* ray, std::map<float, GameObject*>& collisions)const;

public:

	bool Update();

	void SentToDraw(Camera* camera) const;

	void AddComponent(Component* component);

	void Inspector();

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	void Delete(GameObject* to_delete);

	void ChangeMaterial(Material* new_material);
	void ChangeMesh(Mesh * new_mesh);

	bool Hirarchy(GameObject*& selected);

	GameObject* CreateChild(const char* const name = nullptr);
	GameObject* CreateChild(Component* component, const char* const name = nullptr);

	void AddChild(GameObject* child);
	GameObject* CreateCamera();

	GameObject* GetChild(const std::string& name) const;

	//Gets
	const unsigned int GetNumComponents() const;
	const std::string& GetName() const;
	const Mesh* GetMesh() const;
	const Material* GetMaterial() const;
	math::float4x4 GetLocalTransform()const;
	math::float4x4 GetWorldTransform()const;
	math::vec GetMaxPos() const;
	math::vec GetMinPos() const;
		//Position
	void GetLocalPosX(int& x) const;
	void GetLocalPosY(int& y) const;
	void GetLocalPosZ(int& z) const;
	math::vec GetLocalPosition() const;
	void GetWorldPosX(int& x) const;
	void GetWorldPosY(int& y) const;
	void GetWorldPosZ(int& z) const;
	math::vec GetWorldPosition() const;
		//Rotation
	const math::vec& GetLocalRotationEuler() const;
	math::vec GetWorldRotationEuler() const;
	const math::Quat& GetLocalRotationQuat() const;
	math::Quat GetWorldRotationQuat() const;
		//Scale
	void  GetLocalScaleX(int& x) const;
	void  GetLocalScaleY(int& y) const;
	void  GetLocalScaleZ(int& z) const;
	const math::vec&  GetLocalScale(int& x, int& y, int& z) const;
	void  GetWorldScaleX(int& x) const;
	void  GetWorldScaleY(int& y) const;
	void  GetWorldScaleZ(int& z) const;
	const math::vec&  GetWorldScale(int& x, int& y, int& z) const;

	const Sphere* GetBoundingSphere() const;
	const AABB* GetAABB() const;
	const OBB* GetOBB() const;
	bool IsCamera()const;
	//-----

	//Sets
	void SetLocalTransform(const math::float4x4& new_local_transform);
	void SetWorldTransform(const math::float4x4& new_world_transform);
	void SetDraw(bool draw_);

	bool HasMeshFilter() const;
	bool HasAppliedMaterial() const;
	AppliedMaterial* GetAppliedMaterial() const;
	MeshFilter* GetMeshFilter() const;
	Animator* GetAnimator() const;

	void DrawBoundingBoxes() const;
	
	//Remove unique components
	void RemoveMeshFilter();
	void RemoveAppliedMaterial();

	void PickGameObject(const LineSegment* ray, float ray_distance) const;

	bool AddChildsToKDT(KDTreeGO& kdt) const;

	GameObject* GetParent() const;
};

#endif // !GAME_OBJECT