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

bool PrefabImportConfiguration::Config()
{
	if (ImGui::Checkbox("Pre-Transform", &pre_transform))
		return true;

	return false;
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
