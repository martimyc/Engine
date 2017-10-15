#ifndef _MESH
#define _MESH

#include "glew\include\GL\glew.h"
#include "Component.h"

class Material;
namespace MATH_NAMESPACE_NAME
{
	class AABB;
}

class Mesh: public Component
{
private:
	GLuint vertex_id = 0;
	GLuint num_vertices = 0;
	GLfloat* vertices = nullptr;

	GLuint indices_id = 0;
	GLuint num_indices = 0;
	GLuint* indices = nullptr;

	GLuint normals_id = 0;
	GLfloat* normals = nullptr;

	GLuint num_uv_channels = 0;
	GLuint* num_uv_components = nullptr;
	GLuint* uv_ids = nullptr;
	GLfloat** uvs = nullptr;

	GLuint num_color_channels = 0;
	GLuint* color_ids = nullptr;
	GLfloat** colors = nullptr;

	Material* material = nullptr;

public:
	//TODO update
	Mesh(bool enabled = true);
	Mesh(const GLuint vertex_id, const unsigned int num_vertices, float* vertices, const GLuint indices_id, const unsigned int num_indices, unsigned int* indices, const GLuint normals_id, const GLuint uv_id, bool enabled = true);
	Mesh(const Mesh& mesh, bool enabled = true);
	~Mesh();

	void Draw() const;

	//Getters
	void GetVertices(GLuint& id, GLuint& num, GLfloat* all_vertices) const;
	void GetIndices(GLuint& id, GLuint& num, GLuint* all_indices) const;
	void GetUVs(GLuint& num_channels, GLuint* num_components, GLuint* ids, GLuint* num, GLfloat** all_uvs) const;
	void GetNormals(GLuint& id, GLfloat* all_normals) const;
	void GetColors(GLuint& num_channels, GLuint* ids, GLfloat** all_colors)const;
	Material* GetMaterial() const;

	//Setters
	void SetVertices(const GLuint& id, const GLuint& num, GLfloat* all_vertices);
	void SetIndices(const GLuint& id, const GLuint& num, GLuint* all_indices);
	void SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs);
	void SetNormals(const GLuint& id, GLfloat* all_normals);
	void SetColors(const GLuint& num_channels, GLuint* ids, GLfloat** all_colors);
	void SetMaterial(unsigned int pos);

	void Enclose(AABB& bounding_box)const;
};

#endif // !_MESH
