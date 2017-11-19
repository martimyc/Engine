#include "imgui\imgui.h"
#include "Globals.h"
#include "Resource.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"

MaterialImportConfiguration::MaterialImportConfiguration(): include_textures(true), all_textures_in_same_dir(false)
{
	texture_import_config = new TextureImportConfiguration;
	texture_load_config = new TextureLoadConfiguration;
}

MaterialImportConfiguration::MaterialImportConfiguration(const MaterialImportConfiguration & copy): include_textures(copy.include_textures), all_textures_in_same_dir(copy.all_textures_in_same_dir), texture_import_config(copy.texture_import_config), texture_load_config(copy.texture_load_config)
{}

MaterialImportConfiguration::~MaterialImportConfiguration()
{
	DELETE_PTR(texture_import_config);
	DELETE_PTR(texture_load_config);
}

bool MaterialImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::Checkbox("Include Textures", &include_textures))
		ret = true;

	if (ImGui::Checkbox("All textures are in scenes dir", &all_textures_in_same_dir))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("If you export a scene and delete all directory hirarchy after to make a .zip for your students you should load scene with this");

	if (include_textures)
	{
		if (ImGui::TreeNodeEx("Textures' Import Settings", ImGuiTreeNodeFlags_Framed))
		{
			if (texture_import_config->Config())
				ret = true;
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Textures' Load Settings", ImGuiTreeNodeFlags_Framed))
		{
			if (texture_load_config->Config())
				ret = true;
			ImGui::TreePop();
		}
	}

	return ret;
}

void MaterialImportConfiguration::MetaSave(char ** iterator) const
{
	texture_import_config->MetaSave(iterator);
	texture_load_config->MetaSave(iterator);
}

void MaterialImportConfiguration::MetaLoad(char ** iterator)
{
	texture_import_config->MetaLoad(iterator);
	texture_load_config->MetaLoad(iterator);
}

unsigned int MaterialImportConfiguration::GetMetaSize() const
{
	return texture_import_config->GetMetaSize() + texture_load_config->GetMetaSize();
}

bool MaterialLoadConfiguration::Config()
{
	bool ret = false;
	return ret;
}

void MaterialLoadConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void MaterialLoadConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int MaterialLoadConfiguration::GetMetaSize() const
{
	return sizeof(MaterialLoadConfiguration);
}

MaterialAsset::MaterialAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MATERIAL, resource, import_config, load_config)
{}

MaterialAsset::~MaterialAsset()
{
	DELETE_PTR(resource);
}

void MaterialAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}
