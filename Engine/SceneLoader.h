#ifndef _MODULE_SCENE_LOADER
#define _MODULE_SCENE_LOADER

#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "Module.h"

class Mesh;
class Material;

class SceneLoader : public Module
{
private:

public:
	SceneLoader(const char* name, bool start_enabled = true);
	~SceneLoader();

	bool Init();
	bool CleanUp();

	//Loads
	//Scene loads
	bool LoadScene(const char* path) const;
	bool LoadMesh(const aiMesh* mesh, Mesh& new_mesh) const;
	bool LoadVertices(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadIndices(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadTextureCoordinates(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadNormals(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;
	bool LoadColors(const aiMesh* mesh, const GLuint& num_vertices, Mesh& new_mesh, bool equal_size_floats, bool equal_size_uints) const;

	//Material loads
	bool LoadMaterial(const aiMaterial* material, Material& new_material) const;
};

#endif //!_MODULE_SCENE_LOADER