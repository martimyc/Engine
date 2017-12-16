#ifndef SKELETON_ASSET
#define SKELETON_ASSET

#include "Asset.h"

struct SkeletonImportConfiguration;
struct SkeletonLoadConfiguration;
class Skeleton;

class SkeletonAsset : public Asset
{
	std::vector<const GameObject*> instances;
	static GLuint image;

public:
	SkeletonAsset(Skeleton* resource, const SkeletonImportConfiguration* import_config, const SkeletonLoadConfiguration* load_config);
	~SkeletonAsset();

	static void SetImage(GLuint id);
	virtual GLuint GetImage() const;

	void AddInstance(const GameObject* go);
	void DeleteInstance(const GameObject* go);
};

struct SkeletonImportConfiguration : public ImportConfiguration
{
	bool load_bone_weights;
	bool limit_bone_weights;
	bool split_by_bone_count;
	bool debone;
	float debone_threshold;

	SkeletonImportConfiguration();
	SkeletonImportConfiguration(const SkeletonImportConfiguration& copy);
	~SkeletonImportConfiguration();

	bool Config();
	void MetaSave(char** iterator) const;
	void MetaLoad(char** iterator);
	unsigned int GetMetaSize() const;
};

struct SkeletonLoadConfiguration : public LoadConfiguration
{
	bool Config();
	void MetaSave(char** iterator) const;
	void MetaLoad(char** iterator);
	unsigned int GetMetaSize() const;
};

#endif //!SKELETON_ASSET