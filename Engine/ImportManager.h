#ifndef MODULE_IMPORTER
#define MODULE_IMPORTER

#include <string>
#include <vector>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "Module.h"

class MeshImporter;
class MaterialImporter;
class TextureImporter;

struct ImportConfiguration;
struct LoadConfiguration;
struct TextureLoadConfiguration;
struct MaterialLoadConfiguration;
struct MeshLoadConfiguration;

struct UID;

struct TextureSource;

class Material;
class Mesh;

class Resource;
enum RESOURCE_TYPE;

class ImportManager : public Module
{
private:
	std::string import_path;
	RESOURCE_TYPE import_type;
	bool importing = false;
	ImportConfiguration* import_config;
	LoadConfiguration* load_config;
	
	MeshImporter* mesh_importer = nullptr;
	MaterialImporter* material_importer = nullptr;
	TextureImporter* texture_importer = nullptr;

public:
	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

private:
	const UID Import(const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config) const; //should return uid / 0 if it fails
	void GenerateMeta(const std::string& path, const UID& resource_id, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config) const;

	TextureSource* LoadTexture(const UID & uid, const TextureLoadConfiguration* load_config) const;
	//Material* LoadMaterial(const UID & uid, const MaterialLoadConfiguration* load_config) const;
	//Mesh* LoadMesh(const UID & uid, const MeshLoadConfiguration* load_config) const;
	//Scene* LoadScene(const std::string& name) const;

	//Scene
	bool ImportScene(const std::string& path) const;
	//bool LoadScene(const std::string& name) const;

	//Objects
	bool ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<Material*>& materials, bool* material_loads, const std::vector<Mesh*>& meshes, bool* mesh_loads) const;

public:
	bool Init();
	bool CleanUp();

	UPDATE_STATUS Update(float dt);

	void SentToImport(const std::string& path, RESOURCE_TYPE type);

	bool Load(Resource* to_load, const LoadConfiguration* load_config);
};


#endif //!MODULE_IMPORTER
