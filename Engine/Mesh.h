#ifndef _MESH
#define _MESH

#include "glew\include\GL\glew.h"
#include "Component.h"

class Mesh: public Component
{
public:
	GLuint vertex_id = 0;
	GLuint num_indices = 0;
	GLuint indices_id = 0;
	GLuint num_vertices = 0;
	GLuint normals_id = 0;
	GLuint uv_id = 0;

	Mesh(bool enabled = true);
	Mesh(const GLuint vertex_id,const GLuint num_indices,const GLuint indices_id,const GLuint num_vertices,const GLuint normals_id,const GLuint uv_id, bool enabled = true);
	Mesh(const Mesh& mesh, bool enabled = true);
	~Mesh();

	void EnableDraw() const;
	void DisableDraw() const;
};

#endif // !_MESH
