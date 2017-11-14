#ifndef MATERIAL_ASSET
#define MATERIAL_ASSET

#include "Asset.h"

struct TextureImportConfiguration;
struct TextureLoadConfiguration;

struct MaterialImportConfiguration : public ImportConfiguration
{
	TextureImportConfiguration* texture_import_config;

	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

struct MaterialLoadConfiguration : public LoadConfiguration
{
	TextureLoadConfiguration* texture_load_config;

	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

class MaterialAsset: public Asset
{
private:


public:
	MaterialAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~MaterialAsset();
};

#endif // !MATERIAL_ASSET

