#ifndef _MODULE_MESH_LOADER
#define _MODULE_MESH_LOADER

#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
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
	bool LoadScene(const char* path);
	bool LoadMesh(const aiMesh* mesh, Mesh& new_mesh);
};

#endif //!_MODULE_MESH_LOADER