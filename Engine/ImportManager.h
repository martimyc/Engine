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
class PrefabImporter;

struct ImportConfiguration;
struct LoadConfiguration;
struct TextureLoadConfiguration;
struct MaterialLoadConfiguration;
struct MeshLoadConfiguration;
struct SceneImportConfiguration;
struct SceneLoadConfiguration;

struct UID;

class Material;
class Mesh;

class Resource;
enum RESOURCE_TYPE;
struct TextureSource;
struct MaterialSource;
struct MeshSource;
class Asset;

class ImportManager : public Module
{
private:
	std::string import_path;
	RESOURCE_TYPE import_type;
	bool importing = false;
	ImportConfiguration* import_config;
	LoadConfiguration* load_config;
	GLuint importing_img_id;
	unsigned int importing_img_height;
	unsigned int importing_img_width;

	MeshImporter* mesh_importer = nullptr;
	MaterialImporter* material_importer = nullptr;
	TextureImporter* texture_importer = nullptr;
	PrefabImporter* prefab_importer = nullptr;

	friend class ImportClient;

public:

	//Give acces to private funct Import() to importers who need it
	class ImportClient
	{
	private:
		static const UID Import(const ImportManager* importer, const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config);

		friend class MaterialImporter;
	};

	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

private:
	const UID Import(const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config) const; //should return uid / 0 if it fails

	//Meta files
	bool MetaSave(const std::string& file, const UID& resource_id, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config) const;
	Asset* MetaLoad(const std::string& file) const;

	TextureSource* LoadTexture(const UID & uid, const TextureLoadConfiguration* load_config) const;
	MaterialSource* LoadMaterial(const UID & uid, const MaterialLoadConfiguration* load_config, unsigned int priority = 0) const;
	MeshSource* LoadMesh(const UID & uid, const MeshLoadConfiguration* load_config) const;

	//Scene
	const UID ImportScene(const std::string& file, const SceneImportConfiguration* load_config) const;

	//Objects
	bool ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<UID>& materials, bool* material_loads, const std::vector<UID>& meshes, bool* mesh_loads) const;

	const std::string GetImportFileNameNoExtension() const;
	const std::string GetImportFileNameWithExtension() const;

public:
	bool Init();
	bool CleanUp();

	UPDATE_STATUS Update(float dt);

	void SentToImport(const std::string& path, RESOURCE_TYPE type);

	bool Load(Resource* to_load, const LoadConfiguration* load_config);
};


#endif //!MODULE_IMPORTER
