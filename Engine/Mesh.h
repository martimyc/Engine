#ifndef MESH
#define MESH

#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Resource.h"

#define KDT_GRAPH_SIZE 30

class GameObject;
class AppliedMaterial;
class KDTreeTriangle;

class Vertex;

enum RAYCAST
{
	RC_BRUTE_FORCE = 0,
	RC_KDT
};

struct MeshSource
{
	//really only ids necesary if not animated
	GLuint vertex_id = 0;
	GLuint num_vertices = 0;
	GLfloat* vertices = nullptr;

	GLuint indices_id = 0;
	GLuint num_indices = 0;
	GLuint* indices = nullptr;

	GLuint normals_id = 0;
	GLfloat* normals = nullptr;

	bool has_uvs = false;
	GLuint num_uv_channels = 0;
	GLuint* num_uv_components = nullptr;
	GLuint* uv_ids = nullptr;
	GLfloat** uvs = nullptr;

	bool has_vertex_colors = false;
	GLuint num_color_channels = 0;
	GLuint* color_ids = nullptr;
	GLfloat** colors = nullptr;

	GLenum draw_type;

	//-------
	RAYCAST raycast = RC_BRUTE_FORCE;
	int selected_raycast = 0;
	KDTreeTriangle* triangle_kdt = nullptr;
	float raycast_ticks_log[KDT_GRAPH_SIZE] = {};
	float raycast_checks_log[KDT_GRAPH_SIZE] = {};

	MeshSource();
	MeshSource(const MeshSource& copy);
	~MeshSource();

	void Draw(const AppliedMaterial* draw_material = nullptr) const;

	void Inspector();

	void SaveKDT(char ** iterator) const;
	void LoadKDT(char ** iterator);

	//Getters
	//Vertices
	float3 GetVertex(unsigned int vertex_id) const;
	const GLuint GetVerticesID() const;
	const GLuint GetNumVertices() const;
	const GLfloat* GetVertices() const;
	//Indices
	const GLuint GetIndicesID() const;
	const GLuint GetNumIndices() const;
	const GLuint* GetIndices() const;
	//UVs
	const GLuint GetUVsNumChannels() const;
	const GLuint* GetUVsNumComponents() const;
	const GLuint* GetUVsIDs() const;
	const GLfloat*const* GetUVs() const;
	//Normals
	const GLuint GetNormalsID() const;
	const GLfloat* GetNormals() const;
	//Colors
	const GLuint GetColorsNumChannels()const;
	const GLuint* GetColorsIDs()const;
	const GLfloat*const* GetColors()const;

	//Setters
	void SetVertices(const GLuint& id, const GLuint& num, GLfloat* all_vertices);
	void SetIndices(const GLuint& id, const GLuint& num, GLuint* all_indices);
	void SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs);
	void SetNormals(const GLuint& id, GLfloat* all_normals);
	void SetColors(const GLuint& num_channels, GLuint* ids, GLfloat** all_colors);

	//KDT
	void DrawKDT() const;
	void RecalculateKDT();
	float RayCollisionKDT(const LineSegment* ray);
	float CheckTriangleCollision(const LineSegment * ray);
	float RayCollision(const LineSegment* ray);

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;
	math::vec GetMinVec() const;
	float GetWorldMinX(const math::float4x4& world_transform) const;
	float GetWorldMinY(const math::float4x4& world_transform) const;
	float GetWorldMinZ(const math::float4x4& world_transform) const;
	math::vec GetMinWorldVec(const math::float4x4& world_transform) const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;
	math::vec GetMaxVec() const;
	float GetWorldMaxX(const math::float4x4& world_transform) const;
	float GetWorldMaxY(const math::float4x4& world_transform) const;
	float GetWorldMaxZ(const math::float4x4& world_transform) const;
	math::vec GetMaxWorldVec(const math::float4x4& world_transform) const;

	math::vec GetMinXVertex() const;
	math::vec GetMinYVertex() const;
	math::vec GetMinZVertex() const;
	math::vec GetWorldMinXVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMinYVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMinZVertex(const math::float4x4& world_transform) const;

	math::vec GetMaxXVertex() const;
	math::vec GetMaxYVertex() const;
	math::vec GetMaxZVertex() const;
	math::vec GetWorldMaxXVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMaxYVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMaxZVertex(const math::float4x4& world_transform) const;
};

class Mesh : public Resource
{
private:
	MeshSource* source;

public:
	Mesh(const std::string name,const UID& uid);
	Mesh(const Mesh& copy);
	~Mesh();

	void Draw(const AppliedMaterial* draw_material = nullptr) const;

	bool Inspector();

	bool IsLoaded() const;

	void UnLoad();

	//Getters
	//Vertices
	float3 GetVertex(unsigned int vertex_id) const;
	const GLuint GetVerticesID() const;
	const GLuint GetNumVertices() const;
	const GLfloat* GetVertices() const;
		//Indices
	const GLuint GetIndicesID() const;
	const GLuint GetNumIndices() const;
	const GLuint* GetIndices() const;
		//UVs
	const GLuint GetUVsNumChannels() const;
	const GLuint* GetUVsNumComponents() const;
	const GLuint* GetUVsIDs() const;
	const GLfloat*const* GetUVs() const;
		//Normals
	const GLuint GetNormalsID() const;
	const GLfloat* GetNormals() const;
		//Colors
	const GLuint GetColorsNumChannels()const;
	const GLuint* GetColorsIDs()const;
	const GLfloat*const* GetColors()const;

	//Setters
	void SetVertices(const GLuint& id, const GLuint& num, GLfloat* all_vertices);
	void SetIndices(const GLuint& id, const GLuint& num, GLuint* all_indices);
	void SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs);
	void SetNormals(const GLuint& id, GLfloat* all_normals);
	void SetColors(const GLuint& num_channels, GLuint* ids, GLfloat** all_colors);

	//KDT
	void DrawKDT() const;
	void RecalculateKDT();
	float RayCollision(const LineSegment* ray) const;

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;
	math::vec GetMinVec() const;
	float GetWorldMinX(const math::float4x4& world_transform) const;
	float GetWorldMinY(const math::float4x4& world_transform) const;
	float GetWorldMinZ(const math::float4x4& world_transform) const;
	math::vec GetMinWorldVec(const math::float4x4& world_transform) const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;
	math::vec GetMaxVec() const;
	float GetWorldMaxX(const math::float4x4& world_transform) const;
	float GetWorldMaxY(const math::float4x4& world_transform) const;
	float GetWorldMaxZ(const math::float4x4& world_transform) const;
	math::vec GetMaxWorldVec(const math::float4x4& world_transform) const;

	math::vec GetMinXVertex() const;
	math::vec GetMinYVertex() const;
	math::vec GetMinZVertex() const;
	math::vec GetWorldMinXVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMinYVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMinZVertex(const math::float4x4& world_transform) const;


	math::vec GetMaxXVertex() const;
	math::vec GetMaxYVertex() const;
	math::vec GetMaxZVertex() const;
	math::vec GetWorldMaxXVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMaxYVertex(const math::float4x4& world_transform) const;
	math::vec GetWorldMaxZVertex(const math::float4x4& world_transform) const;

	math::vec GetCenter() const;
	math::vec GetWorldCenter(const math::float4x4& world_transform) const;

	void SetSource(MeshSource* source);
	const MeshSource* GetSource() const;
};

#endif // !MESH
