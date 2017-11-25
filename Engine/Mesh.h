#ifndef MESH
#define MESH

#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Resource.h"

class GameObject;
class AppliedMaterial;
class KDTreeVertex;

class Vertex;


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

	//-------

	KDTreeVertex* vertex_kdt = nullptr;

	MeshSource();
	~MeshSource();

	void Draw(const AppliedMaterial* draw_material = nullptr) const;

	void Inspector();

	//Getters
	//Vertices
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
	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle)const;

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

	Vertex GetMinXVertex() const;
	Vertex GetMinYVertex() const;
	Vertex GetMinZVertex() const;

	Vertex GetMaxXVertex() const;
	Vertex GetMaxYVertex() const;
	Vertex GetMaxZVertex() const;

	bool CheckTriangleCollision(const LineSegment * ray, float* distance) const;
};

class Mesh : public Resource
{
private:
	MeshSource* source;

public:
	Mesh(const std::string name,const UID& uid);
	~Mesh();

	void Draw(const AppliedMaterial* draw_material = nullptr) const;

	bool Inspector();

	bool IsLoaded() const;

	//Getters
		//Vertices
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
	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle) const;

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

	Vertex GetMinXVertex() const;
	Vertex GetMinYVertex() const;
	Vertex GetMinZVertex() const;

	Vertex GetMaxXVertex() const;
	Vertex GetMaxYVertex() const;
	Vertex GetMaxZVertex() const;

	//bool CheckTriangleCollision(const LineSegment* ray, float* distance) const;

	void SetSource(MeshSource* source);
};

#endif // !MESH
