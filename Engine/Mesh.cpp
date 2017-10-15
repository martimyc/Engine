#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Application.h"
#include "SceneManager.h"
#include "Material.h"
#include "Mesh.h"

Mesh::Mesh(bool enabled): Component(CT_MESH, enabled), vertex_id(0), num_vertices(0), vertices (nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{}

/*Mesh::Mesh(const Mesh & mesh, bool enabled): Component(CT_MESH, enabled), vertex_id(mesh.vertex_id), num_indices(mesh.num_indices), indices_id(mesh.indices_id), num_vertices(mesh.num_vertices), normals_id(mesh.normals_id)
{}*/

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
	//Enable state & Bind vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Enable state & Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glIndexPointer(GL_UNSIGNED_INT, 0, NULL);

	//Enable state & Bind color
	if (has_vertex_colors)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, indices_id);
		glColorPointer(4, GL_FLOAT, 0, NULL);
	}

	//bind uvs channel 1 for now
	if(has_uvs && material != nullptr)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glBindBuffer(GL_ARRAY_BUFFER, uv_ids[0]);
		//glTexCoordPointer(num_uv_components[0], GL_FLOAT, 0, NULL);

		GLint max_texture_units = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

		for (int i = 0; i < material->NumTextures(); i++)
		{
			if (i > max_texture_units)
			{
				LOG("Can not render more than %i textures with this hardware, remaining textures will not be rendered", max_texture_units);
				break;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glEnable(GL_TEXTURE_2D);

			glBindBuffer(GL_ARRAY_BUFFER, material->GetTextureCoordinateChannel(i));
			glTexCoordPointer(num_uv_components[0], GL_FLOAT, 0, NULL);

			material->AssignTexturePointers(i);
		}
	}

	if (material != nullptr)
	{
		
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	//Disable texture 2D client state & unbind text buffer
	if (material != nullptr)
	{
		for (int i = 0; i < material->NumTextures(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);		
		}
		//glBindTexture(GL_TEXTURE_2D, 0);
	}// maybe different with multitexturing

	 //After draw bind buffer 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indicies buffer unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable state
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
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

	if (num_components != nullptr && num_channels > 0 && ids != nullptr && all_uvs != nullptr)
		has_uvs = true;
	else
		LOG("UVs where not set correctly");
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

	if ( num_channels > 0 && ids != nullptr && all_colors != nullptr)
		has_vertex_colors = true;
	else
		LOG("Colors where not set correctly");
}

void Mesh::SetMaterial(unsigned int pos)
{
	material = App->scene_manager->GetMaterial(pos);
}

void Mesh::Enclose(AABB & bounding_box) const
{
	vec* vec_vert = new vec[num_vertices];
	memcpy(vec_vert, vertices, num_vertices * sizeof(GLfloat) * 3);
	bounding_box.Enclose(vec_vert, num_vertices);
}
