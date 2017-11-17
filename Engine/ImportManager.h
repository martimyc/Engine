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
struct PrefabImportConfiguration;
struct PrefabLoadConfiguration;
struct SceneImportConfiguration;
struct SceneLoadConfiguration;

struct UID;

class Material;
class Mesh;
class Texture;
class Prefab;

enum RESOURCE_TYPE;
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
		static const UID Import(const ImportManager* importer, const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config);

		friend class MaterialImporter;
	};

	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

private:
	const UID Import(const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config) const;

	//Meta files
	bool MetaSave(const std::string& file, const UID& resource_id, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config) const;
	Asset* MetaLoad(const std::string& file) const;

	//Scene
	const UID ImportScene(const std::string& file, const SceneImportConfiguration* load_config) const;

	const std::string GetImportFileNameNoExtension() const;
	const std::string GetImportFileNameWithExtension() const;

public:
	bool Init();
	bool CleanUp();

	UPDATE_STATUS Update(float dt);

	void SentToImport(const std::string& path, RESOURCE_TYPE type);

	bool LoadMaterial(Material * to_load, const MaterialLoadConfiguration * load_config);
	bool LoadTexture(Texture * to_load, const TextureLoadConfiguration * load_config);
	bool LoadMesh(Mesh * to_load, const MeshLoadConfiguration * load_config);
	bool LoadPrefab(Prefab * to_load, const PrefabLoadConfiguration * load_config);
};


#endif //!MODULE_IMPORTER
