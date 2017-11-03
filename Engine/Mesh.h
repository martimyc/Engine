#ifndef _MESH
#define _MESH

#include "glew\include\GL\glew.h"
#include "Asset.h"

class GameObject;
class AppliedMaterial;

class Mesh: public Asset
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

	bool has_uvs = false;
	GLuint num_uv_channels = 0;
	GLuint* num_uv_components = nullptr;
	GLuint* uv_ids = nullptr;
	GLfloat** uvs = nullptr;

	bool has_vertex_colors = false;
	GLuint num_color_channels = 0;
	GLuint* color_ids = nullptr;
	GLfloat** colors = nullptr;
	
public:
	Mesh(const char* const name);
	~Mesh();

	void Draw(const AppliedMaterial* draw_material = nullptr) const;

	void Inspector();

	//Getters
	void GetVertices(GLuint& id, GLuint& num, GLfloat* all_vertices) const;
	const GLfloat* GetVertices(GLuint & id, GLuint & num) const;
	void GetIndices(GLuint& id, GLuint& num, GLuint* all_indices) const;
	void GetUVs(GLuint& num_channels, GLuint* num_components, GLuint* ids, GLuint* num, GLfloat** all_uvs) const;
	void GetNormals(GLuint& id, GLfloat* all_normals) const;
	void GetColors(GLuint& num_channels, GLuint* ids, GLfloat** all_colors)const;

	//Setters
	void SetVertices(const GLuint& id, const GLuint& num, GLfloat* all_vertices);
	void SetIndices(const GLuint& id, const GLuint& num, GLuint* all_indices);
	void SetUVs(const GLuint& num_channels, GLuint* num_components, GLuint* ids, GLfloat** all_uvs);
	void SetNormals(const GLuint& id, GLfloat* all_normals);
	void SetColors(const GLuint& num_channels, GLuint* ids, GLfloat** all_colors);
	void SetMaterial(unsigned int pos);

	//For priority queue in renderer, draw all meshes with the same material and avoid changeing
	bool operator > (const Mesh& mesh) const;
	bool operator < (const Mesh & mesh) const;	
};

#endif // !_MESH
