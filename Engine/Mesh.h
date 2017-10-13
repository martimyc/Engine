#ifndef _MESH
#define _MESH

#include "glew\include\GL\glew.h"
#include "Component.h"

class Mesh: public Component
{
public:
	GLuint vertex_id = 0;
	unsigned int num_vertices = 0;
	float* vertices = nullptr;

	GLuint indices_id = 0;
	unsigned int num_indices = 0;
	unsigned int* indices = nullptr;

	GLuint normals_id = 0;

	GLuint uv_id = 0;

	Mesh(bool enabled = true);
	Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled = true);
	Mesh(const Mesh& mesh, bool enabled = true);
	~Mesh();

	void AssignDrawPointers() const;
};

#endif // !_MESH
