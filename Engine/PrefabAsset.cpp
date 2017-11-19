#include "imgui\imgui.h"
#include "Resource.h"
#include "PrefabAsset.h"

PrefabAsset::PrefabAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config): Asset(RT_PREFAB, resource, import_config, load_config)
{}

PrefabAsset::~PrefabAsset()
{
	delete resource;
}

void PrefabAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

PrefabImportConfiguration::PrefabImportConfiguration(): pre_transform(true), split_large_meshes(true), sort_by_type(false)
{}

bool PrefabImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::Checkbox("Pre-Transform", &pre_transform))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Removes the node graph and pre-transforms all vertices with the local transformation matrices of their nodes.\nThe output scene still contains nodes, however there is only a root node with children, each one referencing only one mesh, and each mesh referencing one material.\nAnimations are removed during this step.");

	if (ImGui::Checkbox("Split Large Meshes", &split_large_meshes))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Splits large meshes into smaller sub-meshes.");

	if (ImGui::Checkbox("Sort By Type", &sort_by_type))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("This step splits meshes with more than one primitive type in homogeneous sub-meshes.");

	return ret;
}

void PrefabImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void PrefabImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int PrefabImportConfiguration::GetMetaSize() const
{
	return sizeof(PrefabImportConfiguration);
}

bool PrefabLoadConfiguration::Config()
{
	return false;
}

void PrefabLoadConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void PrefabLoadConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int PrefabLoadConfiguration::GetMetaSize() const
{
	return sizeof(PrefabLoadConfiguration);
}
