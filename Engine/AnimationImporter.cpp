#include "UID.h"
//#include "AnimationAsset.h"
#include "AssetDirectory.h"
#include "AnimationImporter.h"

AnimationImporter::AnimationImporter()
{

}

AnimationImporter::~AnimationImporter()
{

}

unsigned int AnimationImporter::GetTotalSize(const aiAnimation * animation) const
{
	return 0;
}

const UID AnimationImporter::Import(const std::string & scene_path, const aiAnimation * animation, const AnimationImportConfiguration * config, AssetDirectory * dir) const
{
	return UID();
}

bool AnimationImporter::Load(Animation * to_load, unsigned int priority, const AnimationLoadConfiguration * config) const
{
	return false;
}
