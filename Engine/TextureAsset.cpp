#include "imgui\imgui.h"
#include "Globals.h"
#include "Resource.h"
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
{
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_possible_anystropy);
}

TextureLoadConfiguration::TextureLoadConfiguration(const TextureLoadConfiguration & config) : anysotropy(config.anysotropy), anysotropy_level(config.anysotropy_level), max_anysotropy(config.max_anysotropy), mip_mapping(config.mip_mapping)
{
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_possible_anystropy);
	if (anysotropy_level > max_possible_anystropy)
	{
		anysotropy_level = max_possible_anystropy;
		LOG("Anystropy level over max possible anystropy level");
	}
}

void TextureLoadConfiguration::Config()
{
	ImGui::Checkbox("Mip-Mapping", &mip_mapping);

	if (mip_mapping)
	{
		ImGui::Checkbox("Anystropy", &anysotropy);
		if (anysotropy)
			ImGui::Checkbox("Max Anystophy", &max_anysotropy);
		if (!max_anysotropy)
		{
			ImGui::Text("Anystropy level");
			ImGui::SameLine();
			ImGui::SliderFloat("", &anysotropy_level, 0.0f, max_possible_anystropy);
		}
	}
}

TextureAsset::TextureAsset(const LoadConfiguration* config) : Asset(RT_TEXTURE, config)
{}

TextureAsset::~TextureAsset()
{}