#ifndef TEXTURE_ASSET
#define TEXTURE_ASSET

#include "glew\include\GL\glew.h"
#include "Asset.h"

struct TextureImportConfiguration : public ImportConfiguration
{
	enum TEXTURE_FORMATS
	{
		TF_NORMAL_FORMATS = 0,
		TF_TGA,
		TF_JPG,
		TF_PNG,
		TF_BMP,
		TF_SGI
	};
	
	enum DXT
	{
		DXT1 = 0x0706,
		DXT2 = 0x0707,
		DXT3 = 0x0708,
		DXT4 = 0x0709,
		DXT5 = 0x070A,
		DXT_NO_COMP = 0x070B,
		KEEP_DXTC_DATA = 0x070C,
		DXTC_DATA_FORMAT = 0x070D,
		THREE_DC = 0x070E,
		RXGB = 0x070F,
		ATI1N = 0x0710,
		DXT1A = 0x0711
	};

	TEXTURE_FORMATS format = TF_NORMAL_FORMATS;
	bool tga_stamp = false;
	bool copresion_menu = true;
	int selected_compresion = 4;
	DXT dxt_format = DXT5;
	int  jpg_quality = 99; // 0 - 99
	bool interlaced = false;
	bool rle = false;

	TextureImportConfiguration();
	TextureImportConfiguration(const std::string& format);

	bool Config();
	void MetaSave(char* iterator) const;
	void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
	const char* GetCompresionCString() const;
};

struct TextureLoadConfiguration : public LoadConfiguration
{
	enum SCALLING_FILTER
	{
		NEAREST = 0x2601,
		LINEAR = 0x2602,
		BILINEAR = 0x2603,
		SCALE_BOX = 0x2604,
		SCALE_TRIANGLE = 0x2605,
		SCALE_BELL = 0x2606,
		SCALE_BSPLINE = 0x2607,
		SCALE_LANCZOS3 = 0x2608,
		SCALE_MITCHELL = 0x2609,
	};

	int offset_x = 0;
	int offset_y = 0;
	bool mip_mapping;
	bool anysotropy;
	bool max_anysotropy;
	GLfloat max_possible_anystropy;
	float anysotropy_level;
	bool alienify;
	bool blur_average;
	bool blur_gaussian;
	int blur_iterations;
	bool contrast;
	float amount_of_contrast;
	bool equalize;
	bool gamma_correction;
	float gamma_correction_factor;
	bool negative;
	bool noise;
	float noise_tolerance;
	bool pixelize;
	int pixels_across;
	bool sharpen;
	float sharpening_factor;
	int sharpen_iterations;
	bool scaling;
	int scale_width;
	int scale_height;
	int scale_depth;
	int selected_filter;
	SCALLING_FILTER scaling_filter;

	TextureLoadConfiguration();

	bool Config();
	void MetaSave(char* iterator) const;
	void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

class TextureAsset : public Asset
{
private:

public:
	TextureAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	~TextureAsset();
};


#endif // !TEXTURE_ASSET

