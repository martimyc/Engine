#include "imgui\imgui.h"
#include "Resource.h"
#include "MeshAsset.h"

bool MeshImportConfiguration::Config()
{
	bool ret = false;

	ImGui::Columns(2);

	if (ImGui::Checkbox("Generate Normals", &gen_normals))
		ret = true;

	ImGui::NextColumn();

	if (ImGui::Checkbox("Generate Smooth Normals", &gen_smoth_normals))
		ret = true;

	ImGui::Columns(1);

	if(ImGui::Checkbox("Split Large Meshes", &split_large_meshes))
		ret = true;

	if(ImGui::Checkbox("Sort By Type", &sort_by_type))
		ret = true;

	return ret;
}

void MeshImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void MeshImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int MeshImportConfiguration::GetMetaSize() const
{
	return sizeof(MeshLoadConfiguration);
}

bool MeshLoadConfiguration::Config()
{
	return false;
}

void MeshLoadConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void MeshLoadConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int MeshLoadConfiguration::GetMetaSize() const
{
	return sizeof(MeshLoadConfiguration);
}

MeshAsset::MeshAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MESH, resource, import_config, load_config)
{}

MeshAsset::~MeshAsset()
{}

void MeshAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}
