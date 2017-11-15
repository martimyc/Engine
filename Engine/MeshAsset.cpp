#include "imgui\imgui.h"
#include "Resource.h"
#include "MeshAsset.h"

bool MeshImportConfiguration::Config()
{
	return false;
}

void MeshImportConfiguration::MetaSave(char * iterator) const
{}

void MeshImportConfiguration::MetaLoad(char * iterator)
{}

unsigned int MeshImportConfiguration::GetMetaSize() const
{
	return 0;
}

bool MeshLoadConfiguration::Config()
{
	return false;
}

void MeshLoadConfiguration::MetaSave(char * iterator) const
{
}

void MeshLoadConfiguration::MetaLoad(char * iterator)
{
}

unsigned int MeshLoadConfiguration::GetMetaSize() const
{
	return 0;
}

MeshAsset::MeshAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MESH, resource, import_config, load_config)
{}

MeshAsset::~MeshAsset()
{}
