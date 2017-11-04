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

struct Bounds
{
	Sphere sphere_bounding_box;
	AABB aabb_bounding_box;
	math::vec original_aabb_bb_points[2];
	OBB obb_bounding_box;
	OBB original_obb_bounding_box;
};

class GameObject
{
private:
	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	GameObject* const parent;
	bool draw = false;
	Transform* transform = nullptr;
	Bounds bounds;
	/*Sphere sphere_bounding_box;
	AABB aabb_bounding_box;
	math::vec original_aabb_bb_points[2];
	OBB obb_bounding_box;
	*/
	//Bounding Boxes
	void CreateBounds(const Mesh* mesh);
	void UpdateBounds();
	void UpdateBoundsUpwards();
	
public:
	GameObject(GameObject* const parent, const char* name);
	~GameObject();

	bool Update();

	void SentToDraw() const;

	void AddComponent(Component* component);

	void Inspector() const;

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);


	void ChangeMaterial(Material* new_material);

	bool Hirarchy(GameObject*& selected);

	GameObject* CreateChild(const char* const name = nullptr);
	GameObject* CreateChild(Component* component, const char* const name = nullptr);

	void AddChild(GameObject* child);

	//Gets
	const unsigned int GetNumComponents() const;
	const std::string& GetName() const;
	math::float4x4 GetLocalTransform()const;
	const float* GetLocalGLTransform()const;
	math::float4x4 GetWorldTransform()const;
	const float* GetWorldGLTransform()const;
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
	//-----

	//Sets
	void SetTransform(const math::float4x4& new_transform);

	bool HasMeshFilter() const;
	bool HasAppliedMaterial() const;
	const AppliedMaterial* GetAppliedMaterial() const;
	const MeshFilter* GetMeshFilter() const;
	void DrawBoundingBoxes() const;

	//Remove unique components
	void RemoveMeshFilter();
	void RemoveAppliedMaterial();
};

#endif // !_GAME_OBJECT

