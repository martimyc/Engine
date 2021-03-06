#include "imgui\imgui.h"
#include "Globals.h"
#include "UID.h"
#include "Resource.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "TextureAsset.h"
#include "AnimationAsset.h"
#include "SkeletonAsset.h"
#include "Asset.h"

Asset::Asset(RESOURCE_TYPE type, Resource* resource, const ImportConfiguration* import_config, const  LoadConfiguration* load_config) : resource(resource), type(type), import_config(import_config), load_config(load_config)
{}

Asset::Asset(RESOURCE_TYPE type, Resource* resource): resource(resource), type(type), import_config(nullptr), load_config(nullptr)
{}

Asset::Asset(const Asset & copy) : resource(copy.resource), type(copy.type), import_config(copy.import_config), load_config(copy.load_config)
{}

Asset::~Asset()
{
	delete import_config;
	delete load_config;
	delete resource;
}

RESOURCE_TYPE Asset::GetType() const
{
	return type;
}

const ImportConfiguration* Asset::GetImportConfig() const
{
	return import_config;
}

const LoadConfiguration* Asset::GetLoadConfig() const
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

SceneImportConfiguration::SceneImportConfiguration(): include_meshes(true), include_materials(true), include_prefabs(true), include_animations(true), include_lights(false), include_cameras(false), include_bones(true), material_import_config(new MaterialImportConfiguration), mesh_import_config (new MeshImportConfiguration), prefab_import_config(new PrefabImportConfiguration), anim_import_config(new AnimationImportConfiguration), skeleton_import_config(new SkeletonImportConfiguration), material_load_config(new MaterialLoadConfiguration), mesh_load_config(new MeshLoadConfiguration), prefab_load_config(new PrefabLoadConfiguration), anim_load_config(new AnimationLoadConfiguration), skeleton_load_config(new SkeletonLoadConfiguration)
{}

SceneImportConfiguration::SceneImportConfiguration(const SceneImportConfiguration & copy) : include_meshes(copy.include_meshes), include_materials(copy.include_materials), include_prefabs(copy.include_prefabs), include_animations(copy.include_animations), include_lights(copy.include_lights), include_cameras(copy.include_cameras), include_bones(copy.include_bones), material_import_config(copy.material_import_config), mesh_import_config(copy.mesh_import_config), prefab_import_config(copy.prefab_import_config), anim_import_config(copy.anim_import_config), skeleton_import_config(copy.skeleton_import_config), material_load_config(copy.material_load_config), mesh_load_config(copy.mesh_load_config), prefab_load_config(copy.prefab_load_config), anim_load_config(copy.anim_load_config), skeleton_load_config(copy.skeleton_load_config)
{}

SceneImportConfiguration::~SceneImportConfiguration()
{
	delete material_import_config;
	delete mesh_import_config;
	delete prefab_import_config;
	delete anim_import_config;
	delete skeleton_import_config;
	delete material_load_config;
	delete mesh_load_config;
	delete prefab_load_config;
	delete anim_load_config;
	delete skeleton_load_config;
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

	if (ImGui::Checkbox("Include Riggs", &include_bones))
		ret = true;

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

	if (include_animations)
	{
		if (ImGui::TreeNodeEx("Animations' Configuration", ImGuiTreeNodeFlags_Framed))
		{
			if (anim_import_config->Config())
				ret = true;
			if (anim_load_config->Config())
				ret = true;

			ImGui::TreePop();
		}
	}

	if (include_bones)
	{
		if (ImGui::TreeNodeEx("Riggs' Configuration", ImGuiTreeNodeFlags_Framed))
		{
			if (skeleton_import_config->Config())
				ret = true;
			if (skeleton_load_config->Config())
				ret = true;

			ImGui::TreePop();
		}
	}

	return ret;
}

void SceneImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, &include_meshes, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_materials, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_prefabs, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_animations, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_lights, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_cameras, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &include_bones, sizeof(bool));
	*iterator += sizeof(bool);

	material_import_config->MetaSave(iterator);
	mesh_import_config->MetaSave(iterator);
	prefab_import_config->MetaSave(iterator);
	anim_import_config->MetaSave(iterator);
	skeleton_import_config->MetaSave(iterator);

	material_load_config->MetaSave(iterator);
	mesh_load_config->MetaSave(iterator);
	prefab_load_config->MetaSave(iterator);
	anim_load_config->MetaSave(iterator);
	skeleton_load_config->MetaSave(iterator);
}

void SceneImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(&include_meshes, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_materials, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_prefabs, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_animations, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_lights, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_cameras, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&include_bones, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	material_import_config->MetaLoad(iterator);
	mesh_import_config->MetaLoad(iterator);
	prefab_import_config->MetaLoad(iterator);
	anim_import_config->MetaLoad(iterator);
	skeleton_import_config->MetaLoad(iterator);

	material_load_config->MetaLoad(iterator);
	mesh_load_config->MetaLoad(iterator);
	prefab_load_config->MetaLoad(iterator);
	anim_load_config->MetaLoad(iterator);
	skeleton_load_config->MetaLoad(iterator);
}

unsigned int SceneImportConfiguration::GetMetaSize() const
{
	unsigned int size = sizeof(bool) * 7;
	size += material_import_config->GetMetaSize();
	size += mesh_import_config->GetMetaSize();
	size += prefab_import_config->GetMetaSize();
	size += anim_import_config->GetMetaSize();
	size += skeleton_import_config->GetMetaSize();
	size += material_load_config->GetMetaSize();
	size += mesh_load_config->GetMetaSize();
	size += prefab_load_config->GetMetaSize();
	size += anim_load_config->GetMetaSize();
	size += skeleton_load_config->GetMetaSize();
	return size;
}
