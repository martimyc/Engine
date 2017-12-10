#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "Resource.h"
#include "Animation.h"
#include "AnimationAsset.h"

GLuint AnimationAsset::image = 0;

AnimationAsset::AnimationAsset(Animation * resource, const AnimationImportConfiguration * import_config, const AnimationLoadConfiguration * load_config) : Asset(RT_ANIMATION, resource, import_config, load_config)
{}

AnimationAsset::~AnimationAsset()
{}

void AnimationAsset::SetImage(GLuint id)
{
	image = id;
}

GLuint AnimationAsset::GetImage() const
{
	return image;
}

void AnimationAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

AnimationLoadConfiguration::AnimationLoadConfiguration()
{}

AnimationLoadConfiguration::~AnimationLoadConfiguration()
{}

bool AnimationLoadConfiguration::Config()
{
	return false;
}

void AnimationLoadConfiguration::MetaSave(char ** iterator) const
{}

void AnimationLoadConfiguration::MetaLoad(char ** iterator)
{}

unsigned int AnimationLoadConfiguration::GetMetaSize() const
{
	return 0;
}

AnimationImportConfiguration::AnimationImportConfiguration()
{}

AnimationImportConfiguration::~AnimationImportConfiguration()
{}

bool AnimationImportConfiguration::Config()
{
	return false;
}

void AnimationImportConfiguration::MetaSave(char ** iterator) const
{}

void AnimationImportConfiguration::MetaLoad(char ** iterator)
{}

unsigned int AnimationImportConfiguration::GetMetaSize() const
{
	return 0;
}
