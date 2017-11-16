#ifndef MESH
#define MESH

#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Resource.h"

class GameObject;
class AppliedMaterial;
class KDTreeVertex;

namespace Geo
{
	struct Vertex;

	struct Triangle
	{
		const Vertex& first;
		const Vertex& second;
		const Vertex& third;

		Triangle(const Vertex& first, const Vertex& second, const Vertex& third) : first(first), second(second), third(third)
		{}
		~Triangle()
		{}
	};

	struct Vertex
	{
		float x;
		float y;
		float z;

		std::vector<const Triangle*> triangles;

		Vertex(const GLfloat* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2])
		{}

		Vertex(float x, float y, float z): x(x), y(y), z(z)
		{}

		~Vertex()
		{
			for (std::vector<const Triangle*>::iterator it = triangles.begin(); it != triangles.end(); ++it)
				if (*it != nullptr)
					delete *it;
			triangles.clear();
		}

		float operator [] (int i)
		{
			if (i == 0)
				return x;
			if (i == 1)
				return y;
			if (i == 2)
				return z;
		}

		float operator [] (int i) const
		{
			if (i == 0)
				return x;
			if (i == 1)
				return y;
			if (i == 2)
				return z;
		}

		math::vec MGLVec() const
		{
			return math::vec(x,y,z);
		}

		void AddTriangle(const Triangle* new_triangle)
		{
			triangles.push_back(new_triangle);
		}
	};
}

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

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

	Geo::Vertex GetMinXVertex() const;
	Geo::Vertex GetMinYVertex() const;
	Geo::Vertex GetMinZVertex() const;

	Geo::Vertex GetMaxXVertex() const;
	Geo::Vertex GetMaxYVertex() const;
	Geo::Vertex GetMaxZVertex() const;

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

	//Max & Min
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

	Geo::Vertex GetMinXVertex() const;
	Geo::Vertex GetMinYVertex() const;
	Geo::Vertex GetMinZVertex() const;

	Geo::Vertex GetMaxXVertex() const;
	Geo::Vertex GetMaxYVertex() const;
	Geo::Vertex GetMaxZVertex() const;

	bool CheckTriangleCollision(const LineSegment* ray, float* distance) const;

	void SetSource(MeshSource* source);
};

#endif // !MESH
