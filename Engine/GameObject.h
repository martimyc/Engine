#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include <string>
#include "glew\include\GL\glew.h"

class Component;
class Transform;
class AppliedMaterial;
class MeshFilter;
class Material;

class GameObject
{
private:
	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	const GameObject* const parent;
	bool draw = false;

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

	bool Hirarchy( GameObject*& selected);

	GameObject* CreateChild();
	GameObject* CreateChild(Component* component);

	void AddChild(GameObject* child);

	//Gets
	const unsigned int GetNumComponents() const;
	const std::string& GetName() const;

	bool HasMeshFilter() const;
	bool HasAppliedMaterial() const;
	const AppliedMaterial* GetAppliedMaterial() const;
	const MeshFilter* GetMeshFilter() const;

	//Remove unique components
	void RemoveMeshFilter();
	void RemoveAppliedMaterial();

	//Creates for all components
	Transform* CreateTransformation(const char* const name = nullptr);

};

#endif // !_GAME_OBJECT

