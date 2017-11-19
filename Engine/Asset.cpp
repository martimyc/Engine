#include "imgui\imgui.h"
#include "Globals.h"
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
	if (import_config != nullptr)
	{
		delete import_config;
	}

	if (load_config != nullptr)
	{
		delete load_config;
	}

	if (resource != nullptr)
	{
		delete resource;
	}
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

SceneImportConfiguration::SceneImportConfiguration(): include_meshes(true), include_materials(true), include_prefabs(true), include_animations(false), include_lights(false), include_cameras(false)
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
	if(material_import_config != nullptr)
		DELETE_PTR(material_import_config);
	if (mesh_import_config != nullptr)
		DELETE_PTR(mesh_import_config);
	if (prefab_import_config != nullptr && include_prefabs == false)
		DELETE_PTR(prefab_import_config);
	if (material_load_config != nullptr)
		DELETE_PTR(material_load_config);
	if (mesh_load_config != nullptr)
		DELETE_PTR(mesh_load_config);
	if (prefab_load_config != nullptr && include_prefabs == false)
		DELETE_PTR(prefab_load_config);
}

bool SceneImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::Checkbox("Include Meshes", &include_meshes))
		ret = true;

	if (ImGui::Checkbox("Include Materials", &include_materials))
		ret = true;

	if (ImGui::Checkbox("Include Prefabs", &include_prefabs))
		ret = true;

	if (ImGui::Checkbox("Include Animations", &include_animations))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Not suported yet");

	if (ImGui::Checkbox("Include Lights", &include_lights))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Not working yet");

	if (ImGui::Checkbox("Include Cameras", &include_cameras))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Not working yet");

	if (include_meshes)
	{
		if (ImGui::TreeNodeEx("Meshes' Configuration", ImGuiTreeNodeFlags_Framed))
		{
			if (mesh_import_config->Config())
				ret = true;
			if (mesh_load_config->Config())
				ret = true;

			ImGui::TreePop();
		}
	}
		
	if (include_materials)
	{
		if (ImGui::TreeNodeEx("Materials' Configuration", ImGuiTreeNodeFlags_Framed))
		{
			if (material_import_config->Config())
				ret = true;
			if (material_load_config->Config())
				ret = true;

			ImGui::TreePop();
		}
	}

	if (include_prefabs)
	{
		if (ImGui::TreeNodeEx("Prefabs' Configuration", ImGuiTreeNodeFlags_Framed))
		{
			if (prefab_import_config->Config())
				ret = true;
			if (prefab_load_config->Config())
				ret = true;

			ImGui::TreePop();
		}
	}

	return ret;
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
