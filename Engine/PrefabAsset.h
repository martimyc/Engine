#ifndef PREFAB_ASSET
#define PREFAB_ASSET

#include "Asset.h"
typedef unsigned int GLuint;

struct PrefabImportConfiguration : public ImportConfiguration
{
	bool pre_transform; //make scene only have root with childs with one mesh
	bool split_large_meshes;
	bool sort_by_type;
	bool optimize_graph;
	bool optimize_meshes;

	PrefabImportConfiguration();

	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

struct PrefabLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

class PrefabAsset : public Asset
{
private:
	std::vector<const GameObject*> instances;
	GLuint image;

public:
	PrefabAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~PrefabAsset();

	void AddInstance(const GameObject* go);

	GLuint GetImage() const;
	void SetImage(GLuint id);
};
#endif // !PREFAB_ASSET
