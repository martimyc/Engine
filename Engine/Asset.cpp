#include "imgui\imgui.h"
#include "UID.h"
#include "Resource.h"
#include "Asset.h"
#include "TextureAsset.h"

Asset::Asset(ASSET_TYPE type, const LoadConfiguration* config) : resource(nullptr), type(type), config(config)
{}

Asset::Asset(ASSET_TYPE type, Resource * resource): resource(resource), type(type), config(nullptr)
{}

Asset::~Asset()
{
	delete config;
	if (resource != nullptr)
		delete resource;
}

ASSET_TYPE Asset::GetType() const
{
	return type;
}

const LoadConfiguration * Asset::GetConfig() const
{
	return config;
}

const UID & Asset::GetUID() const
{
	return resource->GetUID();
}

void Asset::SetResource(Resource * new_resource)
{
	resource = new_resource;
}

Resource * Asset::GetResource() const
{
	return resource;
}

const std::string & Asset::GetName() const
{
	return resource->GetName();
}

void Asset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}
