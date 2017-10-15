#include "Globals.h"
#include "Application.h"
#include "SceneManager.h"
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

	for (int i = 0; i < num_uv_channels; i++)
		if (uv_ids[i] != 0)
			glDeleteBuffers(1, &uv_ids[i]);
}

void Mesh::Draw() const
{
	//Enable state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Bind vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glIndexPointer(GL_UNSIGNED_INT, 0, NULL);

	//bind uvs channel 1 for now
	if (uv_ids != nullptr)
	{
		//glBindTexture(GL_TEXTURE_2D, );???
		glBindBuffer(GL_ARRAY_BUFFER, uv_ids[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (material != nullptr)
	{
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
	}// maybe different with multitexturing

	 //After draw bind buffer 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indicies buffer unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable state
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::GetVertices(GLuint & id, GLuint & num, GLfloat * all_vertices) const
{
	id = vertex_id;
	num = num_vertices;
	all_vertices = vertices;
}

void Mesh::GetIndices(GLuint & id, GLuint & num, GLuint * all_indices) const
{
	id = indices_id;
	num = num_indices;
	all_indices = indices;
}

void Mesh::GetUVs(GLuint& num_channels, GLuint* num_components, GLuint* ids, GLuint* num, GLfloat** all_uvs) const
{
	num_channels = num_uv_channels;
	num_components = num_uv_components;
	ids = uv_ids;
	all_uvs = uvs;
}

void Mesh::GetNormals(GLuint & id, GLfloat * all_normals) const
{
	id = normals_id;
	all_normals = normals;
}

void Mesh::GetColors(GLuint & num_channels, GLuint * ids, GLfloat ** all_colors) const
{
	num_channels = num_color_channels;
	ids = color_ids;
	all_colors = colors;
}

Material * Mesh::GetMaterial() const
{
	return material;
}

void Mesh::SetVertices(const GLuint & id, const GLuint & num, GLfloat * all_vertices)
{
	vertex_id = id;
	num_vertices = num;

	if (vertices != nullptr)
		delete[] vertices;

	vertices = all_vertices;
}

void Mesh::SetIndices(const GLuint & id, const GLuint & num, GLuint * all_indices)
{
	indices_id = id;
	num_indices = num;

	if (indices != nullptr)
		delete[] indices;

	indices = all_indices;
}

void Mesh::SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs)
{
	if (num_uv_components != nullptr)
		delete[] num_uv_components;
	num_uv_components = num_components;

	if (uv_ids != nullptr)
		delete[] uv_ids;
	uv_ids = ids;

	if (uvs != nullptr)
	{
		for (int i = 0; i != num_uv_channels; i++)
			delete[] uvs[i];
		delete[] uvs;
	}
	uvs = all_uvs;

	//must be last to delete well if necesary
	num_uv_channels = num_channels;
}

void Mesh::SetNormals(const GLuint & id, GLfloat * all_normals)
{
	normals_id = id;

	if (normals != nullptr)
		delete[] normals;
	normals = all_normals;
}

void Mesh::SetColors(const GLuint & num_channels, GLuint * ids, GLfloat ** all_colors)
{
	if (color_ids != nullptr)
		delete[] color_ids;
	color_ids = ids;

	if (colors != nullptr)
	{
		for (int i = 0; i != num_color_channels; i++)
			delete[] colors[i];
		delete[] colors;
	}
	colors = all_colors;

	//must be last to delete well if necesary
	num_color_channels = num_channels;
}

void Mesh::SetMaterial(unsigned int pos)
{
	material = App->scene_manager->GetMaterial(pos);
}
