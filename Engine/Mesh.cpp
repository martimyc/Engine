#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Application.h"
#include "SceneManager.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"

Mesh::Mesh(bool enabled): Component(CT_MESH, enabled), vertex_id(0), num_vertices(0), vertices (nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{}

/*Mesh::Mesh(const Mesh & mesh, bool enabled): Component(CT_MESH, enabled), vertex_id(mesh.vertex_id), num_indices(mesh.num_indices), indices_id(mesh.indices_id), num_vertices(mesh.num_vertices), normals_id(mesh.normals_id)
{}*/

Mesh::~Mesh()
{
	if (vertex_id != 0)
		glDeleteBuffers(1, &vertex_id);
	if (vertices != nullptr)
		delete[] vertices;

	if (indices_id != 0)
		glDeleteBuffers(1, &indices_id);
	if (indices != nullptr)
		delete[] indices;

	if (normals_id != 0)
		glDeleteBuffers(1, &normals_id);
	if (normals != nullptr)
		delete[] normals;

	for (int i = 0; i < num_uv_channels; i++)
	{
		if (uv_ids[i] != 0)
			glDeleteBuffers(1, &uv_ids[i]);
		if (uvs[i] != nullptr)
			delete uvs[i];
	}
	if (uv_ids != nullptr)
		delete[] uv_ids;
	if (uvs != nullptr)
		delete[] uvs;
	if (num_uv_components != nullptr)
		delete[] num_uv_components;

	if (color_ids != nullptr)
		delete[] color_ids;
	
	for (int i = 0; i < num_color_channels; i++)
	{
		if (color_ids[i] != 0)
			glDeleteBuffers(1, &color_ids[i]);
		if (colors[i] != nullptr)
			delete colors[i];
	}
	if (color_ids != nullptr)
		delete[] color_ids;
	if (colors != nullptr)
		delete[] colors;
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
		GLint max_texture_units = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

		for (int i = 0; i < material->NumTextures(); i++)
		{
			if (i > max_texture_units)
			{
				LOG("Can not render more than %i textures with this hardware, remaining textures will not be rendered", max_texture_units);
				break;
			}

			//Texture units
			glActiveTexture(GL_TEXTURE0 + i);
			//Textures
			glEnable(GL_TEXTURE_2D);
			material->AssignTexturePointers(i);

			//UVs
			GLuint uv_channel = material->GetTextureCoordinateChannel(i);

			glClientActiveTexture(GL_TEXTURE0 + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, uv_ids[uv_channel]);
			glTexCoordPointer(num_uv_components[uv_channel], GL_FLOAT, 0, NULL);
		}
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	//Disable texture 2D client state & unbind text buffer
	if (material != nullptr)
	{
		for (int i = 0; i < material->NumTextures(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);

			glClientActiveTexture(GL_TEXTURE0 + i);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}// maybe different with multitexturing

	 //After draw bind buffer 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indicies buffer unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable state
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh:: Configuration(int num_component)
{
	char mesh_name[255];
	sprintf(mesh_name, "Mesh: %i", num_component);

	if(ImGui::TreeNode(mesh_name))
	{
		ImGui::Text("Vertices: %i", num_vertices);
		ImGui::Text("Indices: %i", num_indices);
		ImGui::Text("UV channels: %i", num_uv_channels);

		if(has_vertex_colors)
			ImGui::Text("Has Vertex Colors");
		else
			ImGui::Text("Does not have Vertex Colors");

		if (material != nullptr)
			material->Configuration(num_uv_channels);
		else
			ImGui::Text("No material aplied to this mesh");
		ImGui::TreePop();
	}
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

void Mesh::ApplyTexture(Texture* text)
{
	if (material != nullptr)
		material->AddTexture(text);
	else
	{
		material = new Material();
		material->AddTexture(text);
	}
}

void Mesh::ChangeMaterial(Material * new_material)
{
	material = new_material;
}
