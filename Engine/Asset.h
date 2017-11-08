#ifndef ASSET
#define ASSET

#include <vector>
#include "Resource.h"

class GameObject;

struct ImportConfiguration
{};

struct LoadConfiguration
{};

struct Asset
{
	Resource* resource;
	LoadConfiguration config;
	std::vector<const GameObject*> instances;

	Asset(Resource* resource);
	~Asset();

	const UID& GetID() const
	{
		return resource->GetUID();
	}

	Resource* UseFor(const GameObject* go)
	{
		instances.push_back(go);
		return resource;
	}

	bool IsInUse() const
	{
		return instances.size() > 0;
	}
};

//Textures
struct TextureImportConfiguration : public ImportConfiguration
{
	enum TEXTURE_FORMATS
	{
		TF_NORMAL_FORMATS = 0,
		TF_JPG,
		TF_PNG,
		TF_RLE_FORMATS
	};

	TEXTURE_FORMATS format;
	unsigned int jpg_quality; // 0 - 99
	bool interlaced;
	bool rle;
	unsigned int offset_x;
	unsigned int offset_y;
};

struct TextureLoadConfiguration : public LoadConfiguration
{
	bool mip_mapping;
	bool anysotropy;
	bool max_anysotropy;
	unsigned int anysotropy_level;
};

//Materials
struct MaterialImportConfiguration : public ImportConfiguration
{
	TextureImportConfiguration* texture_import_config;

};

struct MaterialLoadConfiguration : public LoadConfiguration
{
	TextureLoadConfiguration* texture_load_config;

};

//Meshes
struct MeshImportConfiguration : public ImportConfiguration
{

};

struct MeshLoadConfiguration : public LoadConfiguration
{

};

//Scenes
struct SceneImportConfiguration : public ImportConfiguration
{

};

struct SceneLoadConfiguration : public LoadConfiguration
{

};

#endif // !ASSET

