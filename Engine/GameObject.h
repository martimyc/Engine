#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include <vector>
#include "glew\include\GL\glew.h"

class Component;

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

	std::vector<Components*>
public:
	GameObject();

	~GameObject();

	virtual bool Draw(DRAW_MODE mode) const;
};

#endif // !_GAME_OBJECT

