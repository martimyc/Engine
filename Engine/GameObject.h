#ifndef _GAME_OBJECT
#define _GAME_OBJECT

#include "glew\include\GL\glew.h"

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

	GLuint vertex_id = 0;
	GLuint num_indices = 0;
	GLuint indices_id = 0;
	GLuint num_vertices = 0;
	GLuint normals_id = 0;
	GLuint uv_id = 0;
	GLuint texture_id = 0;

public:
	GameObject();
	GameObject( const GLuint vertex_id, const GLuint indices_id, const GLuint normals_id, GLuint uv_id, const GLuint texture_id);

	~GameObject();

	virtual bool Draw(DRAW_MODE mode) const = 0;
};

#endif // !_GAME_OBJECT

