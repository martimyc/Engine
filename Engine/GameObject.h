#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;
class Mesh;

enum DRAW_MODE
{
	D_NO_DRAW = 0,
	D_NORMAL,
	D_CHECKERS,
	D_WIREFRAME,
	D_NORMALS
};

class GameObject
{
private:
	GLfloat world_position[3];
	GLfloat rotation[3];

	GLuint texture_id = 0;

	std::vector<Component*> components;

	const Mesh* mesh;

public:
	GameObject();
	GameObject(const Mesh* mesh);

	~GameObject();

	virtual void Draw(DRAW_MODE mode, GLuint text) const;
};

#endif // !_GAME_OBJECT

