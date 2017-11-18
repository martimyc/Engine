#ifndef MESH_ASSET
#define MAESH_ASSET

#include "Asset.h"

struct MeshImportConfiguration : public ImportConfiguration
{
	bool gen_normals;
	bool gen_smoth_normals;
	bool split_large_meshes;
	bool sort_by_type;

	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

struct MeshLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

class MeshAsset : public Asset
{
private:
	std::vector<const GameObject*> instances;

public:
	MeshAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~MeshAsset();

	void AddInstance(const GameObject* go);

};
#endif // !MESH_ASSET

