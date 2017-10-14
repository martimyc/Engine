#ifndef _MESH
#define _MESH

#include "glew\include\GL\glew.h"
#include "Component.h"

#define NUM_UV_CHANNELS 4

class Material;

class Mesh: public Component
{
private:
	GLuint vertex_id = 0;
	GLuint num_vertices = 0;
	GLfloat* vertices = nullptr;

	GLuint indices_id = 0;
	GLuint num_indices = 0;
	GLuint* indices = nullptr;

	GLuint normals_id = 0;

	GLuint num_uv_channels = 0;
	GLuint uv_id[NUM_UV_CHANNELS] = {};

	Material* material = nullptr;

public:
	//TODO update
	Mesh(bool enabled = true);
	Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled = true);
	Mesh(const Mesh& mesh, bool enabled = true);
	~Mesh();

	void Draw() const;

	//Getters
	void GetVertices(GLuint& id, GLuint& num, GLfloat* all_vertices) const;
	void GetIndices(GLuint& id, GLuint& num, GLuint* all_indices) const;

	//Setters
	void SetVertices(GLuint& id, GLuint& num, GLfloat* all_vertices);
	void SetIndices(GLuint& id, GLuint& num, GLuint* all_indices);
};

#endif // !_MESH
