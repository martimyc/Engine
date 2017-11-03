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

class GameObject
{
private:
	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	const GameObject* const parent;
	bool draw = false;
	Transform* transform = nullptr;
	Sphere sphere_bounding_box;
	AABB original_AABB_bounding_box;
	AABB AABB_bounding_box;
	OBB OBB_bounding_box;

public:
	GameObject(const GameObject* const parent, const char* name);
	~GameObject();

	bool Update();

	void SentToDraw() const;

	void AddComponent(Component* component);

	void Inspector() const;

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	const float* const GetTransformationMatrix()const;

	void ChangeMaterial(Material* new_material);

	bool Hirarchy(GameObject*& selected);

	GameObject* CreateChild(const char* const name = nullptr);
	GameObject* CreateChild(Component* component, const char* const name = nullptr);

	void AddChild(GameObject* child);

	//Gets
	const unsigned int GetNumComponents() const;
	const std::string& GetName() const;
	void GetWorldPosition(int& x, int& y, int& z) const;
	void GetWorldRotation(int& x, int& y, int& z, int&w) const;
	void GetWorldScale(int& x, int& y, int& z) const;

	bool HasMeshFilter() const;
	bool HasAppliedMaterial() const;
	const AppliedMaterial* GetAppliedMaterial() const;
	const MeshFilter* GetMeshFilter() const;
	void DrawBoundingBoxes() const;

	//Remove unique components
	void RemoveMeshFilter();
	void RemoveAppliedMaterial();

private:
	//Bounding Boxes
	void CreateBounds(const Mesh* mesh);
	void UpdateBounds();
	void DrawAABBBoundingBox() const;


};

#endif // !_GAME_OBJECT

