#ifndef MATERIAL_ASSET
#define MATERIAL_ASSET

#include "Asset.h"

struct TextureImportConfiguration;
struct TextureLoadConfiguration;

class MaterialAsset: public Asset
{
private:
	struct MaterialImportConfiguration : public ImportConfiguration
	{
		TextureImportConfiguration* texture_import_config;

	};

	struct MaterialLoadConfiguration : public LoadConfiguration
	{
		TextureLoadConfiguration* texture_load_config;

	};

public:

};

#endif // !MATERIAL_ASSET

