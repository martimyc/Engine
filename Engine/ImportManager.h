#ifndef MODULE_IMPORTER
#define MODULE_IMPORTER

#include <string>
#include <vector>
#include "Assimp\include\cimport.h"
#include "Assimp\include\config.h"
#include "Assimp\include\Importer.hpp"
#include "Assimp\include\types.h"
#include "Assimp\include\DefaultLogger.hpp"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "Module.h"

class MeshImporter;
class MaterialImporter;
class TextureImporter;
class PrefabImporter;
class AnimationImporter;
class SkeletonImporter;

class AssetDirectory;

struct ImportConfiguration;
struct LoadConfiguration;
struct TextureImportConfiguration;
struct TextureLoadConfiguration;
struct MaterialLoadConfiguration;
struct MeshLoadConfiguration;
struct PrefabImportConfiguration;
struct PrefabLoadConfiguration;
struct SkeletonImportConfiguration;
struct SkeletonLoadConfiguration;
struct AnimationImportConfiguration;
struct AnimationLoadConfiguration;
struct SceneImportConfiguration;

struct UID;

class Material;
class Mesh;
class Texture;
class Prefab;
class Skeleton;
class Animation;

class AssetDirectory;

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
	AnimationImporter* anim_importer = nullptr;
	SkeletonImporter* skeleton_importer = nullptr;

	friend class ImportClient;

public:

	//Give acces to private funct Import() to importers who need it and to AssetDirectory
	class ImportClient
	{
	private:
		static const UID Import(const ImportManager* importer, const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config, AssetDirectory* dir = nullptr);
		static void StraightImport(const std::string& file_path, AssetDirectory* dir);

		friend class MaterialImporter;
		friend class FileSystem;
		friend class AssetDirectory;
	};

	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

private:
	const UID Import(const std::string& path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config, AssetDirectory* dir = nullptr) const;

	//Meta files
	bool TextureMetaSave(const std::string& full_path, const UID& resource_id, const TextureImportConfiguration* import_config, const TextureLoadConfiguration* load_config) const;
	bool SceneMetaSave(const std::string& full_path, const std::vector<std::pair<UID, std::string>>& mesh_uids, const std::vector<std::pair<UID, std::string>>& material_uids, const std::vector<std::pair<UID, std::string>>& skeletons, const std::vector<std::pair<UID, std::string>>& animations, const UID& prefab_id, const SceneImportConfiguration* import_config) const;

	//Scene
	const UID ImportScene(const std::string& file, const SceneImportConfiguration* load_config, AssetDirectory* dir = nullptr) const;
	void LoadScene(AssetDirectory* dir, char ** iterator, const SceneImportConfiguration* config, const std::string& scene_name) const;
	void CollapseDummyNodes(aiNode* node) const;
	aiNode* GetMeshNode(aiNode* root, int num_mesh) const;

	const std::string GetImportFileNameNoExtension() const;
	const std::string GetImportFileNameWithExtension() const;

	void DeleteSceneFiles(char ** iterator) const;

public:
	bool Init();
	bool CleanUp();

	UPDATE_STATUS Update(float dt);

	void SentToImport(const std::string& path, RESOURCE_TYPE type);

	bool LoadMaterial(Material * to_load, const MaterialLoadConfiguration* load_config) const;
	bool LoadTexture(Texture * to_load, const TextureLoadConfiguration * load_config) const;
	bool LoadMesh(Mesh * to_load, const MeshLoadConfiguration* load_config) const;
	bool LoadPrefab(Prefab * to_load, const PrefabLoadConfiguration* load_config) const;
	bool LoadSkeleton(Skeleton * to_load, const SkeletonLoadConfiguration* load_config) const;
	bool LoadAnimation(Animation * to_load, const AnimationLoadConfiguration* load_config) const;

	GLuint GenerateButtonImage(const std::string &relative_path);

	void MetaLoad(const std::string& file, AssetDirectory* dir) const;

	void ReImportWithMeta(time_t file_last_mod, const std::string& meta_file, AssetDirectory* dir) const;

	const UID SaveScene() const;
};


#endif // !MODULE_IMPORTER
