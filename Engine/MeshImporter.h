#ifndef MESH_IMPORTER
#define MESH_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "UID.h"

class Mesh;

struct MeshLoadConfiguration;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

private:
	unsigned int GetTotalSize(const aiMesh* mesh) const;

public:
	const UID Import(const aiMesh* mesh, const std::string& scene_path,const std::string& name);
	Mesh* Load(const UID& id, const MeshLoadConfiguration& config);
};

#endif // !MESH_IMPORTER

