#ifndef PREFAB_IMPORTER
#define PREFAB_IMPORTER

#include <string>
#include <vector>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"

struct Prefab;
struct UID;
struct PrefabImportConfiguration;
struct PrefabLoadConfiguration;
class GameObject;

class PrefabImporter
{
public:
	PrefabImporter();
	~PrefabImporter();

private:
	unsigned int GetFailedBefore(unsigned int pos, bool* loads, unsigned int num_objects) const;
	unsigned int GetNodeSize(const aiNode* node, bool* mesh_loads) const;
	void ImportNode(const aiNode* child, char** iterator, const aiScene* scene, const std::vector<UID>& materials, bool* material_loads, const std::vector<UID>& meshes, bool* mesh_loads, const PrefabImportConfiguration* config);
	GameObject* LoadChild(char** iterator, GameObject* parent);

public:
	//TODO change for config
	const UID Import(const aiScene* scene, const std::vector<UID>& materials, bool* material_loads, const std::vector<UID>& meshes, bool* mesh_loads, const PrefabImportConfiguration* config);
	bool Load(Prefab* to_load, const PrefabLoadConfiguration* config);
};

#endif //!PREFAB_IMPORTER
