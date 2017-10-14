#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;

class GameObject
{
private:
	GLfloat world_position[3];
	GLfloat rotation[3];

	std::vector<Component*> components;

public:
	GameObject();
	~GameObject();

	virtual void Draw() const;

	void AddComponent(Component* component);

	void Reset();
};

#endif // !_GAME_OBJECT

