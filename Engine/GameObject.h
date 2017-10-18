#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;
class Material;
class Texture;

namespace MATH_NAMESPACE_NAME 
{
	class AABB;
}

class GameObject
{
private:
	GLfloat world_position[3] = { 0.0f, 0.0f, 0.0f };
	GLfloat rotation[3];

	std::vector<Component*> components;

	bool inspector_open = true;

public:
	GameObject();
	~GameObject();

	virtual void Draw(bool normals = false) const;

	void AddComponent(Component* component);

	void Configuration() const;

	void ApplyTexture(Texture * text);

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	void GenerateBoundingBox(AABB& bounding_box) const;

	void GetWorldPosition(GLfloat& x, GLfloat& y, GLfloat& z);

	void ChangeMaterial(Material* new_material, int mesh_num);

	uint GetNumComponents() const;

	uint GetNumMeshes() const;
};

#endif // !_GAME_OBJECT

