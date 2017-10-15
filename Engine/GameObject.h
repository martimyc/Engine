#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;
class Texture;

class GameObject
{
private:
	GLfloat world_position[3] = { 0.0f, 0.0f, 0.0f };
	GLfloat rotation[3];

	std::vector<Component*> components;

public:
	GameObject();
	~GameObject();

	virtual void Draw() const;

	void AddComponent(Component* component);

	void Configuration() const;

	void ApplyTexture(Texture * text);

	void Reset();

	void ReserveComponentSpace(const GLuint& num_components);

	void GetWorldPosition(GLfloat& x, GLfloat& y, GLfloat& z);
};

#endif // !_GAME_OBJECT

