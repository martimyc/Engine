#include "imgui\imgui.h"
#include "Globals.h"
#include "Resource.h"
#include "Texture.h"
#include "TextureAsset.h"

TextureImportConfiguration::TextureImportConfiguration() : format (TF_NORMAL_FORMATS), tga_stamp(false), selected_compresion(4), dxt_format(DXT5), copresion_menu(true), jpg_quality(99), interlaced(false), rle(false), transform_uvs(true)
{}

TextureImportConfiguration::TextureImportConfiguration(const TextureImportConfiguration & copy): format(copy.format), tga_stamp(copy.tga_stamp), copresion_menu(copy.copresion_menu), selected_compresion (copy.selected_compresion), dxt_format (copy.dxt_format), jpg_quality (copy.jpg_quality), interlaced (copy.interlaced), rle (copy.rle), transform_uvs(copy.transform_uvs)
{}

TextureImportConfiguration::TextureImportConfiguration(const std::string & format) : format(TF_NORMAL_FORMATS), tga_stamp(false), selected_compresion(4), dxt_format(DXT5), copresion_menu(true), jpg_quality(99), interlaced(false), rle(false)
{
	if (format == "TGA" || format == "tga")
	{
		this->format = TF_TGA;
		return;
	}
	else if (format == "JPG" || format == "jpg")
	{
		this->format = TF_JPG;
		return;
	}
	else if (format == "PNG" || format == "png")
	{
		this->format = TF_PNG;
		return;
	}
	else if (format == "BMP" || format == "bmp")
	{
		this->format = TF_BMP;
		return;
	}
	else if(format == "SGI" || format == "sgi")
	{
		this->format = TF_SGI;
		return;
	}
	this->format = TF_NORMAL_FORMATS;
}

//Texture importer
bool TextureImportConfiguration::Config()
{
	bool changed = false;
	ImGui::Text("Compresion:");
	ImGui::SameLine();

	const char* items[] = { "DXT1", "DXT2", "DXT3", "DXT4", "DXT5", "DXT_NO_COMP", "KEEP_DXTC_DATA", "DXTC_DATA_FORMAT", "THREE_DC", "RXGB", "ATI1N", "DXT1A" };
	if (ImGui::Combo("", &selected_compresion, items, IM_ARRAYSIZE(items)))
	{
		switch (selected_compresion)
		{
		case 0:	dxt_format = DXT1; break;
		case 1:	dxt_format = DXT2; break;
		case 2:	dxt_format = DXT3; break;
		case 3:	dxt_format = DXT4; break;
		case 4:	dxt_format = DXT5; break;
		case 5: dxt_format = DXT_NO_COMP; break;
		case 6: dxt_format = KEEP_DXTC_DATA; break;
		case 7: dxt_format = DXTC_DATA_FORMAT; break;
		case 8: dxt_format = THREE_DC; break;
		case 9:	dxt_format = RXGB; break;
		case 10: dxt_format = ATI1N; break;
		case 11: dxt_format = DXT1A; break;
		default: LOG("Non standard compresion"); break;
		}

		changed = true;
	}

	if (format == TF_TGA)
		if (ImGui::Checkbox("Create stamp", &tga_stamp))
			changed = true;
	if (format == TF_JPG)
		if(ImGui::SliderInt("Quality", &jpg_quality, 0, 99))
			changed = true;
	if (format == TF_PNG)
		if(ImGui::Checkbox("Interlaced", &interlaced))
			changed = true;
	if (format == TF_BMP || format == TF_SGI || format == TF_TGA)
		if(ImGui::Checkbox("RLE", &rle))
			changed = true;
	
	if (ImGui::Checkbox("Transform UVs", &transform_uvs))
		changed = true;	

	return changed;
}

void TextureImportConfiguration::MetaSave(char** iterator) const
{
	memcpy(*iterator, &format, sizeof(TEXTURE_FORMATS));
	*iterator += sizeof(TEXTURE_FORMATS);

	memcpy(*iterator, &tga_stamp, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &copresion_menu, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &selected_compresion, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &dxt_format, sizeof(DXT));
	*iterator += sizeof(DXT);

	memcpy(*iterator, &jpg_quality, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &interlaced, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &rle, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &transform_uvs, sizeof(bool));
	*iterator += sizeof(bool);
}

void TextureImportConfiguration::MetaLoad(char** iterator)
{
	memcpy(&format, *iterator, sizeof(TEXTURE_FORMATS));
	*iterator += sizeof(TEXTURE_FORMATS);

	memcpy(&tga_stamp, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&copresion_menu, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&selected_compresion, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&dxt_format, *iterator, sizeof(DXT));
	*iterator += sizeof(DXT);

	memcpy(&jpg_quality, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&interlaced, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&rle, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&transform_uvs, *iterator, sizeof(bool));
	*iterator += sizeof(bool);
}

unsigned int TextureImportConfiguration::GetMetaSize() const
{
	unsigned int ret = sizeof(TEXTURE_FORMATS);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(int);
	ret += sizeof(DXT);
	ret += sizeof(int);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	return ret;
}

const char * TextureImportConfiguration::GetCompresionCString() const
{
	switch (dxt_format)
	{
	case TextureImportConfiguration::DXT1:
		return "DXT1";
	case TextureImportConfiguration::DXT2:
		return "DXT2";
	case TextureImportConfiguration::DXT3:
		return "DXT3";
	case TextureImportConfiguration::DXT4:
		return "DXT4";
	case TextureImportConfiguration::DXT5:
		return "DXT5";
	case TextureImportConfiguration::DXT_NO_COMP:
		return "DXT_NO_COMP";
	case TextureImportConfiguration::KEEP_DXTC_DATA:
		return "KEEP_DXTC_DATA";
	case TextureImportConfiguration::DXTC_DATA_FORMAT:
		return "DXTC_DATA_FORMAT";
	case TextureImportConfiguration::THREE_DC:
		return "THREE_DC";
	case TextureImportConfiguration::RXGB:
		return "RXGB";
	case TextureImportConfiguration::ATI1N:
		return "ATI1N";
	case TextureImportConfiguration::DXT1A:
		return "DXT1A";
	default:
		return "DXT1";
	}
}

TextureLoadConfiguration::TextureLoadConfiguration(): offset_x(0), offset_y(0), mip_mapping(true), anysotropy(true), max_anysotropy(true), anysotropy_level(0), alienify(false), blur_average(false), blur_gaussian(false), contrast(false), blur_iterations(0), amount_of_contrast(1.0f), equalize(false), gamma_correction(false), gamma_correction_factor(1.0f), negative(false), noise(false), noise_tolerance(0.0f), pixelize(false), pixels_across(1), sharpen(false), sharpening_factor(1.0f), sharpen_iterations(1), scaling(false), scale_width(0), scale_height(0), scale_depth(0), selected_filter(0), scaling_filter(NEAREST)
{
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_possible_anystropy);
}

TextureLoadConfiguration::TextureLoadConfiguration(const TextureLoadConfiguration & copy): offset_x(copy.offset_x), offset_y(copy.offset_y), mip_mapping(copy.mip_mapping), anysotropy(copy.anysotropy), max_anysotropy(copy.max_anysotropy), anysotropy_level(copy.anysotropy_level), alienify(copy.alienify), blur_average(copy.blur_average), blur_gaussian(copy.blur_gaussian), contrast(copy.contrast), blur_iterations(copy.blur_iterations), amount_of_contrast(copy.amount_of_contrast), equalize(copy.equalize), gamma_correction(copy.gamma_correction), gamma_correction_factor(copy.gamma_correction_factor), negative(copy.negative), noise(copy.noise), noise_tolerance(copy.noise_tolerance), pixelize(copy.pixelize), pixels_across(copy.pixels_across), sharpen(copy.sharpen), sharpening_factor(copy.sharpening_factor), sharpen_iterations(copy.sharpen_iterations), scaling(copy.scaling), scale_width(copy.scale_width), scale_height(copy.scale_height), scale_depth(copy.scale_depth), selected_filter(copy.selected_filter), scaling_filter(copy.scaling_filter)
{}

bool TextureLoadConfiguration::Config()
{
	bool changed = false;

	if(ImGui::InputInt("Offset X:", &offset_x, 1, 100, ImGuiInputTextFlags_LabelTop))
		changed = true;

	if(ImGui::InputInt("Offset Y:", &offset_y, 1, 100, ImGuiInputTextFlags_LabelTop))
		changed = true;

	if(ImGui::Checkbox("Mip-Mapping", &mip_mapping))
		changed = true;

	if (mip_mapping)
	{
		if(ImGui::Checkbox("Anystropy", &anysotropy))
			changed = true;

		if (anysotropy)
			if(ImGui::Checkbox("Max Anystophy", &max_anysotropy))
				changed = true;

		if (!max_anysotropy)
		{
			ImGui::Text("Anystropy Level");
			ImGui::SameLine();
			if(ImGui::SliderFloatNoLabel("Anystropy Level", &anysotropy_level, 0.0f, max_possible_anystropy))
				changed = true;
		}
	}

	if (ImGui::TreeNodeEx("Filters", ImGuiTreeNodeFlags_Framed))
	{
		ImGui::Columns(2);

		if (ImGui::Checkbox("Blur Gaussian", &blur_gaussian))
		{
			blur_average = false;
			changed = true;
		}
		ImGui::NextColumn();

		if (ImGui::Checkbox("Blur Average", &blur_average))
		{
			blur_gaussian = false;
			changed = true;
		}

		if (blur_gaussian || blur_average)
		{
			ImGui::Columns(1);
			ImGui::Text("Blur Iterations:");
			if(ImGui::SliderIntNoLabel("Blur Iterations", &blur_iterations, 0, 10))
				changed = true;
			ImGui::Columns(2);
		}

		if(contrast)
		{
			while (ImGui::GetColumnIndex() != 0)
				ImGui::NextColumn();
		}
		else
			ImGui::NextColumn();

		if(ImGui::Checkbox("Contrast", &contrast))
			changed = true;
		ImGui::NextColumn();

		if (contrast)
		{
			ImGui::Columns(1);
			ImGui::Text("Amount of Contrast:");
			if(ImGui::SliderFloatNoLabel("Amount of Contrast", &amount_of_contrast, 0.0f, 1.7f))
				changed = true;
			ImGui::Columns(2);
		}

		if(ImGui::Checkbox("Alienify", &alienify))
			changed = true;
		ImGui::NextColumn();		

		if(ImGui::Checkbox("Equalize", &equalize))
			changed = true;

		if (gamma_correction)
		{
			while (ImGui::GetColumnIndex() != 0)
				ImGui::NextColumn();
		}
		else
			ImGui::NextColumn();

		if(ImGui::Checkbox("Gamma Correction", &gamma_correction))
			changed = true;
		ImGui::NextColumn();

		if (gamma_correction)
		{
			ImGui::Columns(1);
			ImGui::Text("Gamma Correction Factor:");
			if(ImGui::SliderFloatNoLabel("Gamma Correction Factor", &gamma_correction_factor, 0.0f, 3.0f))
				changed = true;
			ImGui::Columns(2);
		}

		if(ImGui::Checkbox("Negative", &negative))
			changed = true;

		if (noise)
		{
			while (ImGui::GetColumnIndex() != 0)
				ImGui::NextColumn();
		}
		else
			ImGui::NextColumn();

		if(ImGui::Checkbox("Noise", &noise))
			changed = true;
		ImGui::NextColumn();

		if (noise)
		{
			ImGui::Columns(1);
			ImGui::Text("Noise Tolerance:");
			if(ImGui::SliderFloatNoLabel("Noise Tolerance", &noise_tolerance, 0.0f, 1.0f))
				changed = true;
			ImGui::Columns(2);
		}

		if(ImGui::Checkbox("Pixelize", &pixelize))
			changed = true;
		ImGui::NextColumn();			

		if (pixelize)
		{
			ImGui::Columns(1);
			ImGui::Text("Pixels Acorss:");
			if(ImGui::SliderIntNoLabel("Pixels Acorss", &pixels_across, 1, 30))
				changed = true;
			ImGui::Columns(2);
		}

		if (sharpen)
		{
			while (ImGui::GetColumnIndex() != 0)
				ImGui::NextColumn();
		}
		else
			ImGui::NextColumn();

		if(ImGui::Checkbox("Sharpen", &sharpen))
			changed = true;
		ImGui::NextColumn();

		if (sharpen)
		{
			ImGui::Columns(1);
			ImGui::Text("Sharpen Factor:");
			if(ImGui::SliderFloatNoLabel("Sharpen Factor", &sharpening_factor, 0.0f, 2.5f))
				changed = true;
			ImGui::Text("Sharpen Iterations:");
			if(ImGui::SliderIntNoLabel("Sharpen Iterations", &sharpen_iterations, 1, 10))
				changed = true;
			ImGui::Columns(2);
		}

		ImGui::Columns(1);
		ImGui::TreePop();
	}

	if(ImGui::Checkbox("Scale", &scaling))
		changed = true;

	if (scaling)
	{
		ImGui::Columns(3);

		if(ImGui::InputInt("Width", &scale_width, 1, 100, ImGuiInputTextFlags_LabelTop))
			changed = true;

		ImGui::NextColumn();

		if(ImGui::InputInt("Height", &scale_height, 1, 100, ImGuiInputTextFlags_LabelTop))
			changed = true;

		ImGui::NextColumn();

		if(ImGui::InputInt("Depth", &scale_depth, 1, 100, ImGuiInputTextFlags_LabelTop))
			changed = true;

		ImGui::Columns(1);

		ImGui::Text("Scaling Filter:");

		const char* items[] = { "Nearest", "Linear", "Bilinear", "Scale Box", "Scale Triangle", "Scale Bell", "Scale B Spline", "Scale Lanczos", "Scale Mitchell"};
		if (ImGui::Combo("", &selected_filter, items, IM_ARRAYSIZE(items)))
		{
			switch (selected_filter)
			{
			case 0:	scaling_filter = NEAREST; break;
			case 1:	scaling_filter = LINEAR; break;
			case 2:	scaling_filter = BILINEAR; break;
			case 3:	scaling_filter = SCALE_BOX; break;
			case 4:	scaling_filter = SCALE_TRIANGLE; break;
			case 5: scaling_filter = SCALE_BELL; break;
			case 6: scaling_filter = SCALE_BSPLINE; break;
			case 7: scaling_filter = SCALE_LANCZOS3; break;
			case 8: scaling_filter = SCALE_MITCHELL; break;
			default: LOG("Non standard scaling filter"); break;
			}

			changed = true;
		}
	}
	return changed;
}

void TextureLoadConfiguration::MetaSave(char** iterator) const
{
	memcpy(*iterator, &offset_x, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &offset_y, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &mip_mapping, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &anysotropy, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &max_anysotropy, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &max_possible_anystropy, sizeof(GLfloat));
	*iterator += sizeof(GLfloat);

	memcpy(*iterator, &anysotropy_level, sizeof(float));
	*iterator += sizeof(float);

	memcpy(*iterator, &alienify, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &blur_average, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &blur_gaussian, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &blur_iterations, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &contrast, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &amount_of_contrast, sizeof(float));
	*iterator += sizeof(float);

	memcpy(*iterator, &equalize, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &gamma_correction, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &gamma_correction_factor, sizeof(float));
	*iterator += sizeof(float);

	memcpy(*iterator, &negative, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &noise, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &noise_tolerance, sizeof(float));
	*iterator += sizeof(float);

	memcpy(*iterator, &pixelize, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &pixels_across, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &sharpen, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &sharpening_factor, sizeof(float));
	*iterator += sizeof(float);

	memcpy(*iterator, &sharpen_iterations, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &scaling, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &scale_width, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &scale_height, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &scale_depth, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &selected_filter, sizeof(int));
	*iterator += sizeof(int);

	memcpy(*iterator, &scaling_filter, sizeof(SCALLING_FILTER));
	*iterator += sizeof(SCALLING_FILTER);
}

void TextureLoadConfiguration::MetaLoad(char** iterator)
{
	memcpy(&offset_x, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&offset_y, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&mip_mapping, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&anysotropy, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&max_anysotropy, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&max_possible_anystropy, *iterator, sizeof(GLfloat));
	*iterator += sizeof(GLfloat);

	memcpy(&anysotropy_level, *iterator, sizeof(float));
	*iterator += sizeof(float);

	memcpy(&alienify, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&blur_average, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&blur_gaussian, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&blur_iterations, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&contrast, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&amount_of_contrast, *iterator, sizeof(float));
	*iterator += sizeof(float);

	memcpy(&equalize, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&gamma_correction, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&gamma_correction_factor, *iterator, sizeof(float));
	*iterator += sizeof(float);

	memcpy(&negative, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&noise, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&noise_tolerance, *iterator, sizeof(float));
	*iterator += sizeof(float);

	memcpy(&pixelize, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&pixels_across, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&sharpen, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&sharpening_factor, *iterator, sizeof(float));
	*iterator += sizeof(float);

	memcpy(&sharpen_iterations, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&scaling, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&scale_width, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&scale_height, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&scale_depth, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&selected_filter, *iterator, sizeof(int));
	*iterator += sizeof(int);

	memcpy(&scaling_filter, *iterator, sizeof(SCALLING_FILTER));
	*iterator += sizeof(SCALLING_FILTER);
}

unsigned int TextureLoadConfiguration::GetMetaSize() const
{
	unsigned int ret = sizeof(int);
	ret += sizeof(int);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(GLfloat);
	ret += sizeof(float);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(int);
	ret += sizeof(bool);
	ret += sizeof(float);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(float);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(float);
	ret += sizeof(bool);
	ret += sizeof(int);
	ret += sizeof(bool);
	ret += sizeof(float);
	ret += sizeof(int);
	ret += sizeof(bool);
	ret += sizeof(int);
	ret += sizeof(int);
	ret += sizeof(int);
	ret += sizeof(int);
	ret += sizeof(SCALLING_FILTER);
	return ret;
}

TextureAsset::TextureAsset(Resource* resource, const ImportConfiguration* import_config, const LoadConfiguration* load_config) : Asset(RT_TEXTURE, resource, import_config, load_config)
{}

TextureAsset::~TextureAsset()
{}

void TextureAsset::AddInstance(const Material * material)
{
	instances.push_back(material);
}

void TextureAsset::DeleteInstance(const Material * material)
{
	for (std::vector<const Material*>::iterator it = instances.begin(); it != instances.end(); ++it)
		if (material == *it)
		{
			instances.erase(it);
			break;
		}

	if (instances.size() == 0)
		resource->UnLoad();
}

GLuint TextureAsset::GetImage() const
{
	return ((Texture*)resource)->GetTextureID();
}
