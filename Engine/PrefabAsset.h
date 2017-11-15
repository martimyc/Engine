#ifndef PREFAB_ASSET
#define PREFAB_ASSET

#include "Asset.h"

struct PrefabImportConfiguration : public ImportConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

struct PrefabLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

class PrefabAsset : public Asset
{
private:


public:
	PrefabAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~PrefabAsset();

};
#endif // !PREFAB_ASSET
