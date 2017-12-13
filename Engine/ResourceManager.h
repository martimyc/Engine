#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include <string>
#include <vector>
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Material;
class Texture;
class Mesh;
class Prefab;
class Skeleton;
class Animation;
class Asset;
class UID;
class GameObject;
class AssetDirectory;

enum RESOURCE_TYPE;
struct ImportConfiguration;
struct LoadConfiguration;

enum BUTTON_TYPE
{
	BT_UNKNOWN = 0,
	BT_PLAY,
	BT_PAUSE,
	BT_PLAY_ONE_FRAME
};

struct Button
{
	GLuint id;
	BUTTON_TYPE type = BT_UNKNOWN;
	uint width;
	uint height;

	Button(const BUTTON_TYPE type, GLuint id, uint width, uint height) : type(type), id(id), width(width), height(height) {}
	~Button() { glDeleteTextures(1, &id); }
};

class ResourceManager: public Module
{
private:
	std::vector<Button*> buttons;
	unsigned int selected;
	AssetDirectory* current_dir;
	AssetDirectory* root_dir;

	//Materials
	unsigned int num_materials = 0;
	unsigned int material_priority = 0;

	//Meshes
	unsigned int num_meshes = 0;

	//Textures
	unsigned int num_textures = 0;
	const Texture* debug_texture;
	bool debug_textures = false;

	//Prefabs
	unsigned int num_prefabs = 0;

public:
	ResourceManager(const char * name, bool start_enabled = true);
	~ResourceManager();

private:
	Texture* GetTexture(const UID& uid);
	Material* GetMaterial(const UID& uid);
	Mesh* GetMesh(const UID& uid);
	Prefab* GetPrefab(const UID& uid);
	Material* CreateEmptyMaterial(const char* name = nullptr);

	Texture* LoadCheckers();

	void LoadButtons();
	void LoadIcons();

public:

	bool Start();

	UPDATE_STATUS Update(float dt);

	bool CleanUp();

	void AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	void DeleteAsset(Asset* to_delete);
	bool Exsists(const UID& id) const;

	Material* UseMaterial(const UID& id, const GameObject* go) const;
	Texture* UseTexture(const UID& id, const Material* material) const;
	Mesh* UseMesh(const UID& id, const GameObject* go) const;
	Prefab* UsePrefab(const UID& id, const GameObject* go) const;
	Skeleton* UseSkeleton(const UID& id, const GameObject* go) const;
	Animation* UseAnimation(const UID& id, const GameObject* go) const;

	//Materials
	unsigned int GetNewMaterialPriority();

	void DebugTextures() const;

	void CreateButtons() const;

	void LoadToScene(Asset* asset);

	void SetRootDir(AssetDirectory* dir);
	void SetCurrentDir(AssetDirectory* dir);

	bool CopyFileToCurrentDir(const std::string& path) const;
	const std::string& GetCurrentDirPath() const;

	void UpdateAssets();
};

#endif // !RESOURCE_MANAGER