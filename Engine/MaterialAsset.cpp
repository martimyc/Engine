#include "imgui\imgui.h"
#include "Resource.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"

bool MaterialImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::TreeNodeEx("Textures' Import Settings"))
	{
		if (texture_import_config->Config())
			ret = true;
		ImGui::TreePop();
	}
	return ret;
}

void MaterialImportConfiguration::MetaSave(char * iterator) const
{
	texture_import_config->MetaSave(iterator);
}

void MaterialImportConfiguration::MetaLoad(char * iterator)
{
	texture_import_config->MetaLoad(iterator);
}

unsigned int MaterialImportConfiguration::GetMetaSize() const
{
	return texture_import_config->GetMetaSize();
}

bool MaterialLoadConfiguration::Config()
{
	bool ret = false;

	if (ImGui::TreeNodeEx("Textures' Load Settings"))
	{
		if (texture_load_config->Config())
			ret = true;
		ImGui::TreePop();
	}
	return ret;
}

void MaterialLoadConfiguration::MetaSave(char * iterator) const
{
	texture_load_config->MetaSave(iterator);
}

void MaterialLoadConfiguration::MetaLoad(char * iterator)
{
	texture_load_config->MetaLoad(iterator);
}

unsigned int MaterialLoadConfiguration::GetMetaSize() const
{
	return texture_load_config->GetMetaSize();
}

MaterialAsset::MaterialAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MATERIAL, resource, import_config, load_config)
{}

MaterialAsset::~MaterialAsset()
{}
