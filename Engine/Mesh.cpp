#include "Globals.h"
#include "Material.h"
#include "Mesh.h"

Mesh::Mesh(bool enabled): Component(CT_MESH, enabled), vertex_id(0), num_vertices(0), vertices (nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{}

Mesh::Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled): Component(CT_MESH, enabled), vertex_id(vertex_id), num_vertices(num_vertices), vertices(vertices), indices_id(indices_id), num_indices(num_indices), indices(indices), normals_id(normals_id)
{}

Mesh::Mesh(const Mesh & mesh, bool enabled): Component(CT_MESH, enabled), vertex_id(mesh.vertex_id), num_indices(mesh.num_indices), indices_id(mesh.indices_id), num_vertices(mesh.num_vertices), normals_id(mesh.normals_id)
{}

Mesh::~Mesh()
{
	if (vertex_id != 0)
		glDeleteBuffers(1, &vertex_id);

	if (indices_id != 0)
		glDeleteBuffers(1, &indices_id);

	if (normals_id != 0)
		glDeleteBuffers(1, &normals_id);

	for (int i = 0; i < NUM_UV_CHANNELS; i++)
		if (uv_id[i] != 0)
			glDeleteBuffers(1, &uv_id[i]);
}

void Mesh::Draw() const
{
	//Enable state
	glEnableClientState(GL_VERTEX_ARRAY);

	//Bind vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glIndexPointer(GL_UNSIGNED_INT, 0, NULL);

	if (material != nullptr)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		material->AssignDrawPointers();
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	//Disable texture 2D client state & unbind text buffer
	if (material != nullptr)
	{
		for (int i = 0; i < material->NumTextures(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}// maybe different with multitexturing

	 //After draw bind buffer 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indicies buffer unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable state
	glDisableClientState(GL_VERTEX_ARRAY);
}
