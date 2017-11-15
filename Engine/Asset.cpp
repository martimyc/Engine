#include "imgui\imgui.h"
#include "UID.h"
#include "Resource.h"
#include "Asset.h"
#include "TextureAsset.h"


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

unsigned int Asset::GetNumInsatances() const
{
	return instances.size();
}

const std::string & Asset::GetName() const
{
	return resource->GetName();
}

void Asset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

bool SceneImportConfiguration::Config()
{
	return false;
}

void SceneImportConfiguration::MetaSave(char * iterator) const
{}

void SceneImportConfiguration::MetaLoad(char * iterator)
{}

unsigned int SceneImportConfiguration::GetMetaSize() const
{
	return 0;
}

bool SceneLoadConfiguration::Config()
{
	return false;
}

void SceneLoadConfiguration::MetaSave(char * iterator) const
{}

void SceneLoadConfiguration::MetaLoad(char * iterator)
{}

unsigned int SceneLoadConfiguration::GetMetaSize() const
{
	return 0;
}
