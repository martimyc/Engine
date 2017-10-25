#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include <string>
#include "glew\include\GL\glew.h"

class Component;
class Material;
class Texture;
class TreeNode;
class Mesh;
class Transform;

namespace MATH_NAMESPACE_NAME 
{
	class AABB;
}

class GameObject
{
private:
	std::string name;
	GLfloat world_position_matrix[9] = {};
	GLfloat rotation[3];
	std::vector<Component*> components;
	bool inspector_open = true;
	const TreeNode* const tree_node;
	bool edit_name = false;

public:
	GameObject(const TreeNode* const node, const char* name);
	~GameObject();

	bool Update();

	void Draw() const;

	void AddComponent(Component* component);

	void Inspector() const;

	void ApplyTexture(Texture * text);

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	const float* const GetTransformationMatrix()const;

	void GenerateBoundingBox(AABB& bounding_box) const;

	void ChangeMaterial(Material* new_material, int mesh_num);

	const uint GetNumComponents() const;
	const uint GetNumMeshes() const;
	const std::string& GetName() const;

	//Creates for all components
	Transform* CreateTransformation(const char* const name = nullptr);
	void DeleteMesh(const Mesh* to_delete);
};

#endif // !_GAME_OBJECT

