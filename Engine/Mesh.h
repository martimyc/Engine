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

	bool Inspector();

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
};

#endif // !_MESH
