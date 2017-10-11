#include "Mesh.h"

Mesh::Mesh(bool enabled): Component(C_MESH, enabled)
{}

Mesh::Mesh(const GLuint vertex_id, const GLuint num_indices, const GLuint indices_id, const GLuint num_vertices, const GLuint normals_id, const GLuint uv_id, bool enabled): Component(C_MESH, enabled), vertex_id(vertex_id), num_indices(num_indices), indices_id(indices_id), num_vertices(num_vertices), normals_id(normals_id), uv_id(uv_id)
{}

Mesh::Mesh(const Mesh & mesh, bool enabled): Component(C_MESH, enabled), vertex_id(mesh.vertex_id), num_indices(mesh.num_indices), indices_id(mesh.indices_id), num_vertices(mesh.num_vertices), normals_id(mesh.normals_id), uv_id(mesh.uv_id)
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

void Mesh::Draw(GLuint texture) const
{
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, texture);

	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glIndexPointer(GL_SHORT, 0, 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

bool Mesh::Update()
{
	return true;
}

void Mesh::SetNumVertices(const GLuint num)
{
	num_vertices = num;
}

void Mesh::SetNumIndices(const GLuint num)
{
	num_indices = num;
}
