#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;
class Mesh;

enum DRAW_MODE
{
	DM_NO_DRAW = 0,
	DM_NORMAL,
	DM_CHECKERS,
	DM_WIREFRAME,
	DM_NORMALS
};

class GameObject
{
private:
	GLfloat world_position[3];
	GLfloat rotation[3];

	GLuint texture_id = 0;

	std::vector<Component*> components;

public:
	GameObject();

	~GameObject();

	virtual void Draw(DRAW_MODE mode) const;

	void AddComponent(Component* component);
};

#endif // !_GAME_OBJECT

