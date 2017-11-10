#ifndef MESH_ASSET
#define MAESH_ASSET

#include "Asset.h"

class MeshAsset : public Asset
{
private:
	struct MeshImportConfiguration : public ImportConfiguration
	{

	};

	struct MeshLoadConfiguration : public LoadConfiguration
	{

	};

public:

};
#endif // !MESH_ASSET

