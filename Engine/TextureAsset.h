#ifndef TEXTURE_ASSET
#define TEXTURE_ASSET

#include "Asset.h"

struct TextureImportConfiguration : public ImportConfiguration
{
	enum TEXTURE_FORMATS
	{
		TF_NORMAL_FORMATS = 0,
		TF_JPG,
		TF_PNG,
		TF_RLE_FORMATS
	};

	TEXTURE_FORMATS format = TF_NORMAL_FORMATS;
	int  jpg_quality = 99; // 0 - 99
	bool interlaced = false;
	bool rle = false;
	int offset_x = 0;
	int offset_y = 0;

	TextureImportConfiguration(const std::string& format);

	void Config();
};

struct TextureLoadConfiguration : public LoadConfiguration
{
	bool mip_mapping;
	bool anysotropy;
	bool max_anysotropy;
	unsigned int anysotropy_level;

	TextureLoadConfiguration();
	TextureLoadConfiguration(const TextureLoadConfiguration& config);

	void Config();
};

class TextureAsset : public Asset
{
private:

public:
	TextureAsset(const LoadConfiguration* config);
	~TextureAsset();
};


#endif // !TEXTURE_ASSET

