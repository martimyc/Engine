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

struct Asset
{
	Resource* resource;
	std::vector<const GameObject*> instances;

	Asset(Resource* resource);
	~Asset();
};

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

	//Materials
	void AddMaterial(Material* new_material);
	Material* CreateEmptyMaterial(const char* const name = nullptr);
	void DeleteMaterial(Material* material_to_delete);

	//Meshes
	void AddMesh(Mesh* new_mesh);
	void DeleteMesh(Mesh* mesh_to_delete);

	//Textures
	const Texture* LoadCheckers();
	void AddTexture(Texture* new_texture);
	void DeleteTexture(Texture* texture_to_delete);

	void DebugTextures() const;
};

#endif // !RESOURCE_MANAGER