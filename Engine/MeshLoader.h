#ifndef _MODULE_MESH_LOADER
#define _MODULE_MESH_LOADER

#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "Module.h"

class Mesh;


class MeshLoader : public Module
{
private:

public:
	MeshLoader(const char* name, bool start_enabled = true);
	~MeshLoader();

	bool Init();
	bool CleanUp();

	//Loads
	bool LoadScene(const char* path) const;
	bool LoadMesh(const aiMesh* mesh, Mesh& new_mesh) const;
	bool LoadVertices(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadIndices(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadTextureCoordinates(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadNormals(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadColors(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
};

#endif //!_MODULE_MESH_LOADER