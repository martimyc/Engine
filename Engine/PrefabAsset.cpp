#include "Resource.h"
#include "PrefabAsset.h"

PrefabAsset::PrefabAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config): Asset(RT_PREFAB, resource, import_config, load_config)
{}

PrefabAsset::~PrefabAsset()
{}

bool PrefabImportConfiguration::Config()
{
	return false;
}

void PrefabImportConfiguration::MetaSave(char * iterator) const
{
}

void PrefabImportConfiguration::MetaLoad(char * iterator)
{
}

unsigned int PrefabImportConfiguration::GetMetaSize() const
{
	return sizeof(PrefabImportConfiguration);
}

bool PrefabLoadConfiguration::Config()
{
	return false;
}

void PrefabLoadConfiguration::MetaSave(char * iterator) const
{
}

void PrefabLoadConfiguration::MetaLoad(char * iterator)
{
}

unsigned int PrefabLoadConfiguration::GetMetaSize() const
{
	return sizeof(PrefabLoadConfiguration);
}
