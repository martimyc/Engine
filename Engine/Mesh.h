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
	unsigned int num_vertices = 0;
	float* vertices = nullptr;

	GLuint indices_id = 0;
	unsigned int num_indices = 0;
	unsigned int* indices = nullptr;

	GLuint normals_id = 0;

	unsigned int num_uv_channels = 0;
	GLuint uv_id[NUM_UV_CHANNELS] = {};

	Material* material = nullptr;

public:
	//TODO update
	Mesh(bool enabled = true);
	Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled = true);
	Mesh(const Mesh& mesh, bool enabled = true);
	~Mesh();

	void Draw() const;
};

#endif // !_MESH
