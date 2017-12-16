#include "imgui\imgui.h"
#include "Resource.h"
#include "Skeleton.h"
#include "SkeletonAsset.h"

GLuint SkeletonAsset::image = 0;

SkeletonAsset::SkeletonAsset(Skeleton * resource, const SkeletonImportConfiguration * import_config, const SkeletonLoadConfiguration * load_config): Asset(RT_SKELETON, resource, import_config, load_config)
{}

SkeletonAsset::~SkeletonAsset()
{}

void SkeletonAsset::SetImage(GLuint id)
{
	image = id;
}

GLuint SkeletonAsset::GetImage() const
{
	return image;
}

void SkeletonAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

void SkeletonAsset::DeleteInstance(const GameObject * go)
{
	for (std::vector<const GameObject*>::iterator it = instances.begin(); it != instances.end(); ++it)
		if (go == *it)
		{
			instances.erase(it);
			break;
		}

	if (instances.size() == 0)
		resource->Unload();
}

SkeletonImportConfiguration::SkeletonImportConfiguration() : load_bone_weights(true), limit_bone_weights(false), split_by_bone_count(false), debone(false), debone_threshold(1.0f)
{}

SkeletonImportConfiguration::SkeletonImportConfiguration(const SkeletonImportConfiguration & copy) : load_bone_weights(copy.load_bone_weights), limit_bone_weights(copy.limit_bone_weights), split_by_bone_count(copy.split_by_bone_count), debone(copy.debone), debone_threshold(copy.debone_threshold)
{}

SkeletonImportConfiguration::~SkeletonImportConfiguration()
{}

bool SkeletonImportConfiguration::Config()
{
	bool ret = false;

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

void SkeletonImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, &load_bone_weights, GetMetaSize());
	*iterator += GetMetaSize();
}

void SkeletonImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(&load_bone_weights, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int SkeletonImportConfiguration::GetMetaSize() const
{
	return sizeof(bool) * 3 + sizeof(float);
}

bool SkeletonLoadConfiguration::Config()
{
	return false;
}

void SkeletonLoadConfiguration::MetaSave(char ** iterator) const
{}

void SkeletonLoadConfiguration::MetaLoad(char ** iterator)
{}

unsigned int SkeletonLoadConfiguration::GetMetaSize() const
{
	return 0;
}
