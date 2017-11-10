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

public:
	ResourceManager(const char * name, bool start_enabled = true);
	~ResourceManager();

private:
	void ApplyToMaterial(Texture * new_text, unsigned int num_material);

public:

	bool Init();

	void AddAsset(Asset* new_asset);
	void DeleteAsset(Asset* to_delete);
	bool Exsists(const UID& id) const;
	Resource* Use(const UID& id, const GameObject* go) const;

	//Textures
	Texture* LoadCheckers();

	void DebugTextures() const;
};

#endif // !RESOURCE_MANAGER