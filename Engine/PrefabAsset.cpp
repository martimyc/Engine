#include "imgui\imgui.h"
#include "Resource.h"
#include "PrefabAsset.h"

PrefabAsset::PrefabAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config): Asset(RT_PREFAB, resource, import_config, load_config)
{}

PrefabAsset::~PrefabAsset()
{}

void PrefabAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

GLuint PrefabAsset::GetImage() const
{
	return 0;
}

void PrefabAsset::SetImage(GLuint id)
{
	image = id;
}

PrefabImportConfiguration::PrefabImportConfiguration(): pre_transform(false), split_large_meshes(false), sort_by_type(false), optimize_graph(false), optimize_meshes(true)
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

	if (ImGui::Checkbox("Optimize Graph", &optimize_graph))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Nodes without animations, bones, lights or cameras assigned are collapsed and joined.\nNode names can be lost during this step.");

	if (ImGui::Checkbox("Optimize Meshes", &optimize_meshes))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("A postprocessing step to reduce the number of meshes.");

	return ret;
}

void PrefabImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, &pre_transform, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &split_large_meshes, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &sort_by_type, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &optimize_graph, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &optimize_meshes, sizeof(bool));
	*iterator += sizeof(bool);
}

void PrefabImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(&pre_transform, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&split_large_meshes, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&sort_by_type, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&optimize_graph, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&optimize_meshes, *iterator, sizeof(bool));
	*iterator += sizeof(bool);
}

unsigned int PrefabImportConfiguration::GetMetaSize() const
{
	return sizeof(bool) * 5;
}

bool PrefabLoadConfiguration::Config()
{
	return false;
}

void PrefabLoadConfiguration::MetaSave(char ** iterator) const
{}

void PrefabLoadConfiguration::MetaLoad(char ** iterator)
{}

unsigned int PrefabLoadConfiguration::GetMetaSize() const
{
	return 0;
}
