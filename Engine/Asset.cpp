#include "imgui\imgui.h"
#include "UID.h"
#include "Resource.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "TextureAsset.h"
#include "Asset.h"

Asset::Asset(RESOURCE_TYPE type, Resource* resource, const ImportConfiguration* import_config, const  LoadConfiguration* load_config) : resource(resource), type(type), import_config(import_config), load_config(load_config)
{}

Asset::Asset(RESOURCE_TYPE type, Resource* resource): resource(resource), type(type), import_config(nullptr), load_config(nullptr)
{}

Asset::~Asset()
{
	delete import_config;
	delete load_config;
	if (resource != nullptr)
		delete resource;
}

RESOURCE_TYPE Asset::GetType() const
{
	return type;
}

const ImportConfiguration * Asset::GetImportConfig() const
{
	return import_config;
}

const LoadConfiguration * Asset::GetLoadConfig() const
{
	return load_config;
}

const UID & Asset::GetUID() const
{
	return resource->GetUID();
}

void Asset::SetResource(Resource * new_resource)
{
	resource = new_resource;
}

Resource* Asset::GetResource() const
{
	return resource;
}

const std::string & Asset::GetName() const
{
	return resource->GetName();
}

SceneImportConfiguration::SceneImportConfiguration()
{
	material_import_config = new MaterialImportConfiguration;
	mesh_import_config = new MeshImportConfiguration;
	prefab_import_config = new PrefabImportConfiguration;
	material_load_config = new MaterialLoadConfiguration;
	mesh_load_config = new MeshLoadConfiguration;
	prefab_load_config = new PrefabLoadConfiguration;
}

SceneImportConfiguration::~SceneImportConfiguration()
{
	delete material_import_config;
	delete mesh_import_config;
	delete prefab_import_config;
	delete material_load_config;
	delete mesh_load_config;
	delete prefab_load_config;
}

bool SceneImportConfiguration::Config()
{
	return false;
}

void SceneImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, sizeof(bool) * 4);
	*iterator += sizeof(bool) * 4;

	material_import_config->MetaSave(iterator);
	mesh_import_config->MetaSave(iterator);
	prefab_import_config->MetaSave(iterator);

	material_load_config->MetaSave(iterator);
	mesh_load_config->MetaSave(iterator);
	prefab_load_config->MetaSave(iterator);
}

void SceneImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, sizeof(bool) * 4);
	*iterator += sizeof(bool) * 4;

	material_import_config->MetaLoad(iterator);
	mesh_import_config->MetaLoad(iterator);
	prefab_import_config->MetaLoad(iterator);

	material_load_config->MetaLoad(iterator);
	mesh_load_config->MetaLoad(iterator);
	prefab_load_config->MetaLoad(iterator);
}

unsigned int SceneImportConfiguration::GetMetaSize() const
{
	unsigned int size = sizeof(bool) * 4;
	size += material_import_config->GetMetaSize();
	size += mesh_import_config->GetMetaSize();
	size += prefab_import_config->GetMetaSize();
	size += material_load_config->GetMetaSize();
	size += mesh_load_config->GetMetaSize();
	size += prefab_load_config->GetMetaSize();
	return size;
}
