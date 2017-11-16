#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include <string>
#include <vector>
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Resource;
class Material;
class Texture;
class Mesh;
class Asset;
class UID;
class GameObject;

enum RESOURCE_TYPE;
struct ImportConfiguration;
struct LoadConfiguration;

class ResourceManager: public Module
{
private:
	std::vector<Asset*> assets;

	//Materials
	unsigned int num_materials = 0;
	unsigned int material_priority = 0;

	//Meshes
	unsigned int num_meshes = 0;

	//Textures
	unsigned int num_textures = 0;
	const Texture* debug_texture;
	bool debug_textures = false;

	friend class Client;

public:
	ResourceManager(const char * name, bool start_enabled = true);
	~ResourceManager();

	class Client
	{
		static Texture* GetTexture(ResourceManager* resource_manager, const UID& uid)
		{
			return resource_manager->GetTexture(uid);
		}
		friend class MaterialImporter;
	};

private:
	void ApplyToMaterial(Texture * new_text, unsigned int num_material);

	Texture* GetTexture(const UID& uid);

	Resource* UseFirst(RESOURCE_TYPE type, const GameObject* go);

public:
	bool Init();

	UPDATE_STATUS Update(float dt);

	void AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	void DeleteAsset(Asset* to_delete);
	bool Exsists(const UID& id) const;
	Resource* Use(const UID& id, const GameObject* go) const;

	//Textures
	Texture* LoadCheckers();

	//Materials
	unsigned int GetNumMaterials();

	void DebugTextures() const;
};

#endif // !RESOURCE_MANAGER