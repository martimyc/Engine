#ifndef MESH_ASSET
#define MAESH_ASSET

#include "Asset.h"

struct MeshImportConfiguration : public ImportConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

struct MeshLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

class MeshAsset : public Asset
{
private:


public:
	MeshAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~MeshAsset();

};
#endif // !MESH_ASSET

