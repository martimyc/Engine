#ifndef MESH_IMPORTER
#define MESH_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"

class Mesh;

class MeshImporter
{
private:
	unsigned int GetTotalSize(const aiMesh* mesh) const;

public:
	MeshImporter();
	~MeshImporter();

	bool Import(const aiMesh* mesh, const std::string& scene_path,const std::string& name);
	bool Load(const std::string& name, Mesh& new_mesh);
};

#endif // !MESH_IMPORTER

