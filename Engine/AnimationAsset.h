#ifndef ANIMATION_ASSET
#define ANIMATION_ASSET

#include "Asset.h"

class Animation;
struct AnimationImportConfiguration;
struct AnimationLoadConfiguration;

class AnimationAsset : public Asset
{
private:
	std::vector<const GameObject*> instances;
	static GLuint image;

public:
	AnimationAsset(Animation* resource, const AnimationImportConfiguration* import_config, const AnimationLoadConfiguration* load_config);
	~AnimationAsset();

	static void SetImage(GLuint id);
	virtual GLuint GetImage() const;

	void AddInstance(const GameObject* go);
};


struct AnimationImportConfiguration: public ImportConfiguration
{
	AnimationImportConfiguration();
	~AnimationImportConfiguration();

	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

struct AnimationLoadConfiguration: public LoadConfiguration
{

	AnimationLoadConfiguration();
	~AnimationLoadConfiguration();

	virtual bool Config();
	virtual void MetaSave(char** iterator) const;
	virtual void MetaLoad(char** iterator);
	virtual unsigned int GetMetaSize() const;
};

#endif // !ANIMATION_ASSET

