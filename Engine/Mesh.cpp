#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"

//Containers
#include "KDTreeVertex.h"

//Assets
#include "GameObject.h"
#include "Texture.h"
#include "Transform.h"
#include "AppliedMaterial.h"
#include "Mesh.h"

//Modules
#include "Globals.h"
#include "Application.h"
#include "SceneManager.h"

Mesh::Mesh(const char* const name): Resource(RT_MESH, name), vertex_id(0), num_vertices(0), vertices (nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{}

Mesh::Mesh(const std::string & name): Resource(RT_MESH, name), vertex_id(0), num_vertices(0), vertices(nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{}

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

void Mesh::Draw(const AppliedMaterial* draw_material) const
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
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, normals_id);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//bind uvs channel 1 for now
	if(has_uvs && draw_material != nullptr)
		for (int i = 0; i < draw_material->GetNumTextures(); i++)
		{
			//UVs
			GLuint uv_channel = draw_material->GetUVChannel(i);

			glClientActiveTexture(GL_TEXTURE0 + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, uv_ids[uv_channel]);
			glTexCoordPointer(num_uv_components[uv_channel], GL_FLOAT, 0, NULL);
		}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	//Disable texture 2D client state & unbind text buffer
	if (draw_material != nullptr)
		for (int i = 0; i < draw_material->GetNumTextures(); i++)
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	 //After draw bind buffer 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indicies buffer unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if (has_vertex_colors)
		glDisableClientState(GL_COLOR_ARRAY);
}

bool Mesh::Inspector()
{
	bool ret = true;

	ImGui::Text("Name: %s", name.c_str());
	ImGui::Text("Vertices: %i", num_vertices);
	ImGui::Text("Indices: %i", num_indices);
	ImGui::Text("UV channels: %i", num_uv_channels);

	int size = sizeof(*vertex_kdt);

	if(has_vertex_colors)
		ImGui::Text("Has %i Vertex Color channels", num_color_channels);
	else
		ImGui::Text("Does not have Vertex Colors");

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting mesh");
		ret = false;
	}

	return ret;
}

const GLuint Mesh::GetVerticesID() const
{
	return vertex_id;
}

const GLuint Mesh::GetNumVertices() const
{
	return num_vertices;
}

const GLfloat* Mesh::GetVertices() const
{
	return vertices;
}

const GLuint Mesh::GetIndicesID() const
{
	return indices_id;
}

const GLuint Mesh::GetNumIndices() const
{
	return num_indices;
}

const GLuint * Mesh::GetIndices() const
{
	return indices;
}

const GLuint Mesh::GetUVsNumChannels() const
{
	return num_uv_channels;
}

const GLuint * Mesh::GetUVsNumComponents() const
{
	return num_uv_components;
}

const GLuint * Mesh::GetUVsIDs() const
{
	return uv_ids;
}

const GLfloat * const* Mesh::GetUVs() const
{
	return uvs;
}

const GLuint Mesh::GetNormalsID() const
{
	return normals_id;
}

const GLfloat * Mesh::GetNormals() const
{
	return normals;
}

const GLuint Mesh::GetColorsNumChannels() const
{
	return num_color_channels;
}

const GLuint * Mesh::GetColorsIDs() const
{
	return color_ids;
}

const GLfloat * const * Mesh::GetColors() const
{
	return colors;
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

void Mesh::DrawKDT() const
{
	if (vertex_kdt != nullptr)
		vertex_kdt->Draw();
}

void Mesh::RecalculateKDT()
{
	if (vertex_kdt != nullptr)
		delete vertex_kdt;

	vertex_kdt = new KDTreeVertex();
	vertex_kdt->AddVertices(vertices, num_vertices, indices, num_indices);
}

float Mesh::GetMinX() const
{
	float min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3])
			min = vertices[i * 3];

	return min;
}

float Mesh::GetMinY() const
{
	float min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3 + 1])
			min = vertices[i * 3 + 1];

	return min;
}

float Mesh::GetMinZ() const
{
	float min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3 + 2])
			min = vertices[i * 3 + 2];

	return min;
}

float Mesh::GetMaxX() const
{
	float max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3])
			max = vertices[i * 3];

	return max;
}

float Mesh::GetMaxY() const
{
	float max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3 + 1])
			max = vertices[i * 3 + 1];

	return max;
}

float Mesh::GetMaxZ() const
{
	float max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3 + 2])
			max = vertices[i * 3 + 2];

	return max;
}

Geo::Vertex Mesh::GetMinXVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3])
			min = i;

	return 	Geo::Vertex(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

Geo::Vertex Mesh::GetMinYVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3 + 1])
			min = i;

	return 	Geo::Vertex(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

Geo::Vertex Mesh::GetMinZVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (min < vertices[i * 3 + 2])
			min = i;

	return 	Geo::Vertex(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

Geo::Vertex Mesh::GetMaxXVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3])
			max = i;

	return 	Geo::Vertex(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}

Geo::Vertex Mesh::GetMaxYVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3 + 1])
			max = i;

	return 	Geo::Vertex(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}

Geo::Vertex Mesh::GetMaxZVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (max > vertices[i * 3 + 2])
			max = i;

	return 	Geo::Vertex(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}
