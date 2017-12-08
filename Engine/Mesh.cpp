#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "MathGeoLib\src\Geometry\Triangle.h"

//Containers
#include "KDTreeTriangle.h"

#include "Timer.h"

//Modules
#include "Globals.h"
#include "Application.h"
#include "SceneManager.h"

//Assets
#include "AppliedMaterial.h"
#include "Mesh.h"
#include "Material.h"

MeshSource::MeshSource(): vertex_id(0), num_vertices(0), vertices(nullptr), indices_id(0), num_indices(0), indices(nullptr), normals_id(0), num_uv_channels(0)
{
	memset(raycast_ticks_log, 0.0f, KDT_GRAPH_SIZE * sizeof(float));
	memset(raycast_checks_log, 0.0f, KDT_GRAPH_SIZE * sizeof(float));
}

MeshSource::~MeshSource()
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


void MeshSource::Draw(const AppliedMaterial* draw_material) const
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
	if (has_uvs && draw_material != nullptr)
		for (int i = 0; i < draw_material->GetMaterial()->GetNumTextures(TT_DIFFUSE); i++)
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
		for (int i = 0; i < draw_material->GetMaterial()->GetNumTextures(TT_DIFFUSE); i++)
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

void MeshSource::Inspector()
{
	ImGui::Text("Vertices: %i", num_vertices);
	ImGui::Text("Indices: %i", num_indices);
	ImGui::Text("UV channels: %i", num_uv_channels);

	if (has_vertex_colors)
		ImGui::Text("Has %i Vertex Color channels", num_color_channels);
	else
		ImGui::Text("Does not have Vertex Colors");

	if (ImGui::TreeNode("KDT"))
	{
		if (triangle_kdt != nullptr)
		{
			triangle_kdt->Inspector();


			ImGui::Text("Raycast:");
			ImGui::SameLine();

			const char* items[] = { "Brute Force", "KDT" };
			if (ImGui::Combo("", &selected_raycast, items, IM_ARRAYSIZE(items)))
			{
				switch (selected_raycast)
				{
				case 0:	raycast = RC_BRUTE_FORCE; break;
				case 1:	raycast = RC_KDT; break;
				}
			}

			ImGui::Text("Number of Checks per Raycast:");
			char raycast_checks_title[25];
			sprintf_s(raycast_checks_title, 25, "Checks: %.1f", raycast_checks_log[KDT_GRAPH_SIZE - 1]);
			ImGui::PlotHistogram("     ", raycast_checks_log, IM_ARRAYSIZE(raycast_checks_log), KDT_GRAPH_SIZE, raycast_checks_title, 0.0f, 80.0f, ImVec2(0, 100));

			ImGui::Text("Time per Raycast:");
			char raycast_ticks_title[25];
			sprintf_s(raycast_ticks_title, 25, "Ticks: %.1f", raycast_ticks_log[KDT_GRAPH_SIZE - 1]);
			ImGui::PlotHistogram("      ", raycast_ticks_log, IM_ARRAYSIZE(raycast_ticks_log), KDT_GRAPH_SIZE, raycast_ticks_title, 0.0f, 80.0f, ImVec2(0, 100));

			if (ImGui::Button("Recalculate"))
				RecalculateKDT();
		}
		else
		{
			ImGui::Text("KDT has not been calculated yet");
			if (ImGui::Button("Calculate"))
				RecalculateKDT();
		}

		ImGui::TreePop();
	}
}

void MeshSource::SaveKDT(char ** iterator) const
{
	triangle_kdt->Save(iterator);
}

void MeshSource::LoadKDT(char ** iterator)
{
	triangle_kdt = new KDTreeTriangle;
	triangle_kdt->Load(iterator);
}

const GLuint MeshSource::GetVerticesID() const
{
	return vertex_id;
}

const GLuint MeshSource::GetNumVertices() const
{
	return num_vertices;
}

const GLfloat* MeshSource::GetVertices() const
{
	return vertices;
}

const GLuint MeshSource::GetIndicesID() const
{
	return indices_id;
}

const GLuint MeshSource::GetNumIndices() const
{
	return num_indices;
}

const GLuint * MeshSource::GetIndices() const
{
	return indices;
}

const GLuint MeshSource::GetUVsNumChannels() const
{
	return num_uv_channels;
}

const GLuint * MeshSource::GetUVsNumComponents() const
{
	return num_uv_components;
}

const GLuint * MeshSource::GetUVsIDs() const
{
	return uv_ids;
}

const GLfloat * const* MeshSource::GetUVs() const
{
	return uvs;
}

const GLuint MeshSource::GetNormalsID() const
{
	return normals_id;
}

const GLfloat * MeshSource::GetNormals() const
{
	return normals;
}

const GLuint MeshSource::GetColorsNumChannels() const
{
	return num_color_channels;
}

const GLuint * MeshSource::GetColorsIDs() const
{
	return color_ids;
}

const GLfloat * const * MeshSource::GetColors() const
{
	return colors;
}

void MeshSource::SetVertices(const GLuint & id, const GLuint & num, GLfloat * all_vertices)
{
	vertex_id = id;
	num_vertices = num;

	if (vertices != nullptr)
		delete[] vertices;

	vertices = all_vertices;
}

void MeshSource::SetIndices(const GLuint & id, const GLuint & num, GLuint * all_indices)
{
	indices_id = id;
	num_indices = num;

	if (indices != nullptr)
		delete[] indices;

	indices = all_indices;
}

void MeshSource::SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs)
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

void MeshSource::SetNormals(const GLuint & id, GLfloat * all_normals)
{
	normals_id = id;

	if (normals != nullptr)
		delete[] normals;
	normals = all_normals;
}

void MeshSource::SetColors(const GLuint & num_channels, GLuint * ids, GLfloat ** all_colors)
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

	if (num_channels > 0 && ids != nullptr && all_colors != nullptr)
		has_vertex_colors = true;
	else
		LOG("Colors where not set correctly");
}

void MeshSource::DrawKDT() const
{
	if (triangle_kdt != nullptr)
		triangle_kdt->Draw();
}

void MeshSource::RecalculateKDT()
{
	if (triangle_kdt != nullptr)
		delete triangle_kdt;

	triangle_kdt = new KDTreeTriangle();
	triangle_kdt->AddTriangles(vertices, indices, num_indices, GetMaxVec(), GetMinVec());
}

float MeshSource::RayCollisionKDT(const LineSegment* ray)
{
	float ret = ray->Length();

	if (triangle_kdt != nullptr)
	{
		for (unsigned int i = 0; i < KDT_GRAPH_SIZE - 1; i++)
			raycast_checks_log[i] = raycast_checks_log[i + 1];

		unsigned int checks = 0;
		ret = triangle_kdt->RayCollisionKDT(ray, checks);

		raycast_checks_log[KDT_GRAPH_SIZE - 1] = checks;
	}
	LOG("No KDT to check collisions");

	return ret;
}

float MeshSource::GetMinX() const
{
	float min = inf;

	for (int i = 0; i < num_vertices; i++)
		if (min > vertices[i * 3])
			min = vertices[i * 3];

	return min;
}

float MeshSource::GetMinY() const
{
	float min = inf;

	for (int i = 0; i < num_vertices; i++)
		if (min > vertices[i * 3 + 1])
			min = vertices[i * 3 + 1];

	return min;
}

float MeshSource::GetMinZ() const
{
	float min = inf;

	for (int i = 0; i < num_vertices; i++)
		if (min > vertices[i * 3 + 2])
			min = vertices[i * 3 + 2];

	return min;
}

math::vec MeshSource::GetMaxVec() const
{
	return math::vec(GetMaxX(), GetMaxY(), GetMaxZ());
}

float MeshSource::GetMaxX() const
{
	float max = -inf;

	for (int i = 0; i < num_vertices; i++)
		if (max < vertices[i * 3])
			max = vertices[i * 3];

	return max;
}

float MeshSource::GetMaxY() const
{
	float max = -inf;

	for (int i = 0; i < num_vertices; i++)
		if (max < vertices[i * 3 + 1])
			max = vertices[i * 3 + 1];

	return max; 
}

float MeshSource::GetMaxZ() const
{
	float max = -inf;

	for (int i = 0; i < num_vertices; i++)
		if (max < vertices[i * 3 + 2])
			max = vertices[i * 3 + 2];

	return max;
}

math::vec MeshSource::GetMinVec() const
{
	return math::vec(GetMinX(), GetMinY(), GetMinZ());
}

math::vec MeshSource::GetMinXVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[min * 3] > vertices[i * 3])
			min = i;

	return 	math::vec(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

math::vec MeshSource::GetMinYVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[min * 3 + 1] > vertices[i * 3 + 1])
			min = i;

	return 	math::vec(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

math::vec MeshSource::GetMinZVertex() const
{
	int min = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[min * 3 + 2] > vertices[i * 3 + 2])
			min = i;

	return 	math::vec(vertices[min * 3], vertices[min * 3 + 1], vertices[min * 3 + 2]);
}

math::vec MeshSource::GetMaxXVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[max * 3] < vertices[i * 3])
			max = i;

	return 	math::vec(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}

math::vec MeshSource::GetMaxYVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[max * 3 + 1] < vertices[i * 3 + 1])
			max = i;

	return 	math::vec(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}

math::vec MeshSource::GetMaxZVertex() const
{
	int max = 0.0f;

	for (int i = 0; i < num_vertices; i++)
		if (vertices[max * 3 + 2] < vertices[i * 3 + 2])
			max = i;

	return 	math::vec(vertices[max * 3], vertices[max * 3 + 1], vertices[max * 3 + 2]);
}

float MeshSource::CheckTriangleCollision(const LineSegment * ray)
{
	for (unsigned int i = 0; i < KDT_GRAPH_SIZE - 1; i++)
		raycast_checks_log[i] = raycast_checks_log[i + 1];

	raycast_checks_log[KDT_GRAPH_SIZE - 1] = 0.0f;

	float shortest_distance = ray->Length();
	float distance = 0.0f;

	unsigned int num_triangles = num_indices / 3;
	math::Triangle triangle_to_test;

	for (int i = 0; i < num_triangles; i++)
	{	
		unsigned int index_1 = indices[i * 3];
		unsigned int index_2 = indices[i * 3 + 1];
		unsigned int index_3 = indices[i * 3 + 2];

		memcpy(&triangle_to_test.a, &vertices[index_1], sizeof(float) * 3);
		memcpy(&triangle_to_test.b, &vertices[index_2], sizeof(float) * 3);
		memcpy(&triangle_to_test.c, &vertices[index_3], sizeof(float) * 3);

		//Check all mesh triangles
		if (ray->Intersects(triangle_to_test, &distance, nullptr))
			if (distance < shortest_distance)
				shortest_distance = distance;

		raycast_checks_log[KDT_GRAPH_SIZE - 1]++;
	}

	return distance;
}

float MeshSource::RayCollision(const LineSegment * ray)
{
	Timer timer;

	float ret = 0.0f;

	if (raycast == RC_BRUTE_FORCE)
		ret = CheckTriangleCollision(ray);
	else if (raycast == RC_KDT)
		ret = RayCollisionKDT(ray);

	for (unsigned int i = 0; i < KDT_GRAPH_SIZE - 1; i++)
		raycast_ticks_log[i] = raycast_ticks_log[i + 1];

	raycast_ticks_log[KDT_GRAPH_SIZE - 1] = timer.Read();

	return ret;
}

//Mesh
Mesh::Mesh(const std::string name, const UID& uid):Resource(RT_MESH, name, uid), source(nullptr)
{}

Mesh::~Mesh()
{
	if (source != nullptr)
		delete source;
}

void Mesh::Draw(const AppliedMaterial * draw_material) const
{
	if (source != nullptr)
		source->Draw(draw_material);
	else
		LOG("Trying to acces non loaded mesh");
}

bool Mesh::Inspector()
{
	bool ret = true;

	if (source != nullptr)
		source->Inspector();
	else
		LOG("Trying to acces non loaded mesh");

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting mesh");
		ret = false;
	}

	return ret;
}

bool Mesh::IsLoaded() const
{
	return source != nullptr;
}

const GLuint Mesh::GetVerticesID() const
{
	if (source != nullptr)
		return source->GetVerticesID();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLuint Mesh::GetNumVertices() const
{
	if (source != nullptr)
		return source->GetNumVertices();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLfloat * Mesh::GetVertices() const
{
	if (source != nullptr)
		return source->GetVertices();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLuint Mesh::GetIndicesID() const
{
	if (source != nullptr)
		return source->GetIndicesID();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLuint Mesh::GetNumIndices() const
{
	if (source != nullptr)
		return source->GetNumIndices();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLuint * Mesh::GetIndices() const
{
	if (source != nullptr)
		return source->GetIndices();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLuint Mesh::GetUVsNumChannels() const
{
	if (source != nullptr)
		return source->GetUVsNumChannels();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLuint * Mesh::GetUVsNumComponents() const
{
	if (source != nullptr)
		return source->GetUVsNumComponents();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLuint * Mesh::GetUVsIDs() const
{
	if (source != nullptr)
		return source->GetUVsIDs();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLfloat * const * Mesh::GetUVs() const
{
	if (source != nullptr)
		return source->GetUVs();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLuint Mesh::GetNormalsID() const
{
	if (source != nullptr)
		return source->GetNormalsID();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLfloat * Mesh::GetNormals() const
{
	if (source != nullptr)
		return source->GetNormals();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLuint Mesh::GetColorsNumChannels() const
{
	if (source != nullptr)
		return source->GetColorsNumChannels();
	LOG("Trying to acces non loaded mesh");
	return 0;
}

const GLuint * Mesh::GetColorsIDs() const
{
	if (source != nullptr)
		return source->GetColorsIDs();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

const GLfloat * const * Mesh::GetColors() const
{
	if (source != nullptr)
		return source->GetColors();
	LOG("Trying to acces non loaded mesh");
	return nullptr;
}

void Mesh::SetVertices(const GLuint & id, const GLuint & num, GLfloat * all_vertices)
{
	if (source != nullptr)
		source->SetVertices(id, num, all_vertices);
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::SetIndices(const GLuint & id, const GLuint & num, GLuint * all_indices)
{
	if (source != nullptr)
		source->SetIndices(id, num, all_indices);
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::SetUVs(const GLuint & num_channels, GLuint * num_components, GLuint * ids, GLfloat ** all_uvs)
{
	if (source != nullptr)
		source->SetUVs(num_channels, num_components, ids, all_uvs);
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::SetNormals(const GLuint & id, GLfloat * all_normals)
{
	if (source != nullptr)
		source->SetNormals(id, all_normals);
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::SetColors(const GLuint & num_channels, GLuint * ids, GLfloat ** all_colors)
{
	if (source != nullptr)
		source->SetColors(num_channels, ids, all_colors);
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::DrawKDT() const
{
	if (source != nullptr)
		source->DrawKDT();
	else
		LOG("Trying to acces non loaded mesh");
}

void Mesh::RecalculateKDT()
{
	if (source != nullptr)
		source->RecalculateKDT();
	else
		LOG("Trying to acces non loaded mesh");
}

float Mesh::RayCollision(const LineSegment* ray) const
{
	if (source != nullptr)
		return source->RayCollision(ray);

	LOG("Trying to acces non loaded mesh");
	return false;
}

float Mesh::GetMinX() const
{
	if (source != nullptr)
		return source->GetMinX();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

float Mesh::GetMinY() const
{
	if (source != nullptr)
		return source->GetMinY();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

float Mesh::GetMinZ() const
{
	if (source != nullptr)
		return source->GetMinZ();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

math::vec Mesh::GetMinVec() const
{
	if (source != nullptr)
		return source->GetMinVec();
	LOG("Trying to acces non loaded mesh");
	return math::vec::zero;
}

float Mesh::GetMaxX() const
{
	if (source != nullptr)
		return source->GetMaxX();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

float Mesh::GetMaxY() const
{
	if (source != nullptr)
		return source->GetMaxY();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

float Mesh::GetMaxZ() const
{
	if (source != nullptr)
		return source->GetMaxZ();
	LOG("Trying to acces non loaded mesh");
	return 0.0f;
}

math::vec Mesh::GetMaxVec() const
{
	if (source != nullptr)
		return source->GetMaxVec();
	LOG("Trying to acces non loaded mesh");
	return math::vec::zero;
}

math::vec Mesh::GetMinXVertex() const
{
	if (source != nullptr)
		return source->GetMinXVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetMinYVertex() const
{
	if (source != nullptr)
		return source->GetMinYVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetMinZVertex() const
{
	if (source != nullptr)
		return source->GetMinZVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetMaxXVertex() const
{
	if (source != nullptr)
		return source->GetMaxXVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetMaxYVertex() const
{
	if (source != nullptr)
		return source->GetMaxYVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetMaxZVertex() const
{
	if (source != nullptr)
		return source->GetMaxZVertex();
	LOG("Trying to acces non loaded mesh");
	return math::vec(0.0f, 0.0f, 0.0f);
}

math::vec Mesh::GetCenter() const
{
	return math::vec((GetMaxX() + GetMinX()) / 2, (GetMaxY() + GetMinY()) / 2, (GetMaxZ() + GetMinZ()) / 2);
}

/*bool Mesh::CheckTriangleCollision(const LineSegment * ray, float* distance) const
{
	if (source != nullptr)
		return source->CheckTriangleCollision(ray, distance);
	LOG("Trying to acces non loaded mesh");
	return false;
}*/

void Mesh::SetSource(MeshSource * source)
{
	this->source = source;
}