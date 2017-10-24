#ifndef MESH_IMPORTER
#define MESH_IMPORTER

#include <string>

class Mesh;

class MeshImporter
{
private:

public:
	MeshImporter();
	~MeshImporter();

	bool Import(const std::string& dir);
	bool Load(const std::string& path, Mesh& new_texture);
};

#endif // !MESH_IMPORTER

