#include "imgui\imgui.h"
#include "TextureAsset.h"

TextureImportConfiguration::TextureImportConfiguration(const std::string & format)
{
	if (format == "JPG" || format == "jpg")
	{
		this->format = TF_JPG;
		return;
	}
	else if (format == "PNG" || format == "png")
	{
		this->format = TF_PNG;
		return;
	}
	else if (format == "TGA" || format == "tga" ||
		format == "BMP" || format == "bmp" ||
		format == "SGI" || format == "sgi")
	{
		this->format = TF_RLE_FORMATS;
		return;
	}
	this->format = TF_NORMAL_FORMATS;
}

//Texture importer
void TextureImportConfiguration::Config()
{
	if (format == TF_JPG)
		ImGui::SliderInt("Quality", &jpg_quality, 0, 99, "%i");
	if (format == TF_PNG)
		ImGui::Checkbox("Interlaced", &interlaced);
	if (format = TF_RLE_FORMATS)
		ImGui::Checkbox("RLE", &rle);

	ImGui::Text("Offset X:");
	ImGui::SameLine();
	ImGui::InputInt("", &offset_x);
	ImGui::Text("Offset Y:");
	ImGui::SameLine();
	ImGui::InputInt("", &offset_y);
}

TextureLoadConfiguration::TextureLoadConfiguration(): mip_mapping(true), anysotropy(true), max_anysotropy(true), anysotropy_level(0)
{}

TextureLoadConfiguration::TextureLoadConfiguration(const TextureLoadConfiguration & config) : anysotropy(config.anysotropy), anysotropy_level(config.anysotropy_level), max_anysotropy(config.max_anysotropy), mip_mapping(config.mip_mapping)
{}

void TextureLoadConfiguration::Config()
{}

TextureAsset::TextureAsset(const LoadConfiguration* config) : Asset(AT_TEXTURE, config)
{}

TextureAsset::~TextureAsset()
{}