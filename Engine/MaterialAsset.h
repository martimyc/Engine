#ifndef MATERIAL_ASSET
#define MATERIAL_ASSET

#include "Asset.h"

struct TextureImportConfiguration;
struct TextureLoadConfiguration;

struct MaterialImportConfiguration : public ImportConfiguration
{
	bool include_textures;

	TextureImportConfiguration* texture_import_config;
	TextureLoadConfiguration* texture_load_config; //this is in import to create all texture assets

	MaterialImportConfiguration();
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

public:
	MaterialAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~MaterialAsset();

	void AddInstance(const GameObject* go);
};

#endif // !MATERIAL_ASSET

