#include "Globals.h"
#include "Mesh.h"

Mesh::Mesh(bool enabled): Component(CT_MESH, enabled), vertex_id(0), num_vertices(0), vertices (nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), uv_id (0)
{}

Mesh::Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled): Component(CT_MESH, enabled), vertex_id(vertex_id), num_vertices(num_vertices), vertices(vertices), indices_id(indices_id), num_indices(num_indices), indices(indices), normals_id(normals_id), uv_id(uv_id)
{}

Mesh::Mesh(const Mesh & mesh, bool enabled): Component(CT_MESH, enabled), vertex_id(mesh.vertex_id), num_indices(mesh.num_indices), indices_id(mesh.indices_id), num_vertices(mesh.num_vertices), normals_id(mesh.normals_id), uv_id(mesh.uv_id)
{}

Mesh::~Mesh()
{
	if (vertex_id != 0)
		glDeleteBuffers(1, &vertex_id);
	if (indices_id != 0)
		glDeleteBuffers(1, &indices_id);
	if (normals_id != 0)
		glDeleteBuffers(1, &normals_id);
	if (uv_id != 0)
		glDeleteBuffers(1, &uv_id);
}

void Mesh::AssignDrawPointers() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glIndexPointer(GL_UNSIGNED_INT, 0, NULL);
}
