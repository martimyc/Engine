#ifndef MATERIAL_ASSET
#define MATERIAL_ASSET

#include "Asset.h"

typedef unsigned int GLuint;

struct TextureImportConfiguration;
struct TextureLoadConfiguration;

struct MaterialImportConfiguration : public ImportConfiguration
{
	bool include_textures;
	bool all_textures_in_same_dir;

	TextureImportConfiguration* texture_import_config;
	TextureLoadConfiguration* texture_load_config; //this is in import to create all texture assets

	MaterialImportConfiguration();
	MaterialImportConfiguration(const MaterialImportConfiguration& copy);
	~MaterialImportConfiguration();

	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

struct MaterialLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

class MaterialAsset: public Asset
{
private:
	std::vector<const GameObject*> instances;
	GLuint image;

public:
	MaterialAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~MaterialAsset();

	void AddInstance(const GameObject* go);

	GLuint GetImage() const;
	void SetImage( GLuint id);
};

#endif // !MATERIAL_ASSET

