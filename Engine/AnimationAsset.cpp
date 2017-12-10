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

AnimationImportConfiguration::AnimationImportConfiguration() : load_bone_weights(true), limit_bone_weights(false), split_by_bone_count(false), debone(false), debone_threshold(1.0f)
{}

AnimationImportConfiguration::~AnimationImportConfiguration()
{}

bool AnimationImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::Checkbox("Load Bone Weights", &load_bone_weights))
		ret = true;

	if (ImGui::Checkbox("Limit Bone Weights", &limit_bone_weights))
		ret = true;

	if (ImGui::Checkbox("Split by Bone Count", &split_by_bone_count))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("This step splits meshes with many bones into sub-meshes so that each su-bmesh has fewer or as many bones as a given limit.");

	if (ImGui::Checkbox("Debone", &debone))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("This step removes bones losslessly or according to some threshold.");

	if (debone)
	{
		ImGui::Text("Debone Threshold");
		ImGui::SliderFloatNoLabel("Debone Threshold", &debone_threshold, 0.0f, 10.0f);
	}

	return ret;
}

void AnimationImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, &load_bone_weights, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &limit_bone_weights, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &split_by_bone_count, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &debone, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &debone_threshold, sizeof(float));
	*iterator += sizeof(float);
}

void AnimationImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(&load_bone_weights, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&limit_bone_weights, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&split_by_bone_count, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&debone, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&debone_threshold, *iterator, sizeof(float));
	*iterator += sizeof(float);
}

unsigned int AnimationImportConfiguration::GetMetaSize() const
{
	return sizeof(bool) * 4 + sizeof(float);
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
