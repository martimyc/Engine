#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include <string>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Geometry\OBB.h"
#include "MathGeoLib\src\Geometry\Sphere.h"

class Component;
class Transform;
class AppliedMaterial;
class MeshFilter;
class Material;
class Mesh;
class Camera;
class KDTreeGO;

namespace math
{
	class LineSegment;
}

struct Bounds
{
//	Sphere sphere_bounding_box;
	AABB aabb_bounding_box;
	math::vec original_aabb_bb_points[2];
//	OBB obb_bounding_box;
//	OBB original_obb_bounding_box;


	Bounds()
	{
		aabb_bounding_box.SetNegativeInfinity();
		original_aabb_bb_points[0].zero;
		original_aabb_bb_points[1].zero;
	}

	Bounds(const Bounds& copy): aabb_bounding_box(copy.aabb_bounding_box)
	{
		original_aabb_bb_points[0] = copy.original_aabb_bb_points[0];
		original_aabb_bb_points[1] = copy.original_aabb_bb_points[1];
	}
};

class GameObject
{
private:
	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	GameObject* parent = nullptr;
	Transform* local_transform = nullptr;
	Transform* world_transform = nullptr;
	Bounds bounds;
	bool is_camera = false;
	bool draw = false;

public:
	GameObject(const std::string& name, bool draw = true);
	GameObject(const GameObject& copy);
	~GameObject();

private:
	//Bounding Boxes
	void CreateBounds(const Mesh* mesh);
	void UpdateBounds();
	void UpdateBoundsChilds();
	void UpdateBoundsParents();

	void UpdateAABBs();
	void UpdateWorldTransform(const math::float4x4& parent_world_transform);
	void UpdateTransforms();

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

	const AABB* GetAABB() const;
	bool IsCamera()const;
	//-----

	//Sets
	void SetLocalTransform(const math::float4x4& new_local_transform);
	void SetWorldTransform(const math::float4x4& new_world_transform);
	void SetDraw(bool draw_);

	bool HasMeshFilter() const;
	bool HasAppliedMaterial() const;
	const AppliedMaterial* GetAppliedMaterial() const;
	const MeshFilter* GetMeshFilter() const;
	void DrawBoundingBoxes() const;
	
	//Remove unique components
	void RemoveMeshFilter();
	void RemoveAppliedMaterial();

	void PickGameObject(const LineSegment* ray, float ray_distance) const;

	bool AddChildsToKDT(KDTreeGO& kdt) const;
};

#endif // !_GAME_OBJECT

