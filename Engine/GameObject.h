#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;
class Material;
class Texture;
class TreeNode;

namespace MATH_NAMESPACE_NAME 
{
	class AABB;
}

class GameObject
{
private:
	GLfloat world_position_matrix[9] = {};
	GLfloat rotation[3];

	std::vector<Component*> components;

	bool inspector_open = true;

	const TreeNode* const tree_node;

public:
	GameObject(const TreeNode* const node);
	~GameObject();

	virtual void Draw(bool normals = false) const;

	void AddComponent(Component* component);

	void Inspector() const;

	void ApplyTexture(Texture * text);

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	void GenerateBoundingBox(AABB& bounding_box) const;

	void ChangeMaterial(Material* new_material, int mesh_num);

	const uint GetNumComponents() const;

	const uint GetNumMeshes() const;
};

#endif // !_GAME_OBJECT

