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

	Resource* UseFor(const GameObject* go)
	{
		instances.push_back(go);
		return resource;
	}

	Asset(Resource* resource);
	~Asset();
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

};

struct MaterialLoadConfiguration : public LoadConfiguration
{

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

