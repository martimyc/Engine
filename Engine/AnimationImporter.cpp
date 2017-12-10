#include "Globals.h"
#include "UID.h"
#include "AnimationAsset.h"
#include "AssetDirectory.h"
#include "AnimationImporter.h"

AnimationImporter::AnimationImporter()
{}

AnimationImporter::~AnimationImporter()
{}

unsigned int AnimationImporter::GetTotalSize(const aiAnimation * animation) const
{
	return 0;
}

const UID AnimationImporter::Import(const std::string & scene_path, const aiAnimation * animation, const AnimationImportConfiguration * config, AssetDirectory * dir) const
{
	char* buffer = new char[GetTotalSize(animation)];
	char* iterator = buffer;

	char format[FORMAT_SIZE] = FORMAT_ANIMATION;
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	memcpy(iterator, animation->mName.C_Str(), strlen(animation->mName.C_Str()) + 1);
	iterator += strlen(animation->mName.C_Str()) + 1;

	//ImportSkeleton(animation->);

	return UID();
}

bool AnimationImporter::Load(Animation * to_load, unsigned int priority, const AnimationLoadConfiguration * config) const
{
	return false;
}
