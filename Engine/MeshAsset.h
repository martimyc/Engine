#ifndef MESH_ASSET
#define MAESH_ASSET

#include "Asset.h"

struct MeshImportConfiguration : public ImportConfiguration
{
	bool load_tangents;
	bool gen_tangents;
	bool load_normals;
	bool gen_normals;
	bool gen_smooth_normals;
	bool load_colors;
	bool load_uvs;
	bool load_bone_weights;

	MeshImportConfiguration();

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

