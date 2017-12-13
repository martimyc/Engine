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
	unsigned int GetFailedBefore(unsigned int pos, bool* loads) const;
	unsigned int GetNodeSize(const aiScene* scene, const aiNode* node, bool* mesh_loads, bool* material_loads, bool* skeleton_loads, const char* name = nullptr) const;
	void ImportNode(const aiNode* child, char** iterator, const aiScene* scene, const std::vector<std::pair<UID, std::string>>& materials, bool* material_loads, const std::vector<std::pair<UID, std::string>>& meshes, bool* mesh_loads, const std::vector<std::pair<UID, std::string>> skeletons, bool* skeleton_loads, const char* name = nullptr);
	GameObject* LoadChild(char** iterator);

public:
	//TODO change for config
	const UID Import(const aiScene* scene, const std::vector<std::pair<UID, std::string>>& materials, bool* material_loads, const std::vector<std::pair<UID, std::string>>& meshes, bool* mesh_loads, const std::vector<std::pair<UID, std::string>> skeletons, bool* skeleton_loads, const char* name = nullptr);
	bool Load(Prefab* to_load, const PrefabLoadConfiguration* config);
};

#endif //!PREFAB_IMPORTER
