#include "imgui\imgui.h"

#include "UID.h"

//Resources
#include "Texture.h"
#include "TextureAsset.h"

//Modules
#include "FileSystem.h"
#include "ResourceManager.h"
#include "Application.h"
#include "TextureImporter.h"

TextureImporter::TextureImporter()
{
	// ----- Initialize DevIL libraries -----

	// DevIL sanity check
	if ((iluGetInteger(IL_VERSION_NUM) < IL_VERSION) || (iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) || (ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION))
		LOG("DevIL versions are different... Exiting.");

	//  ----- Initialise DevIL -----
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

TextureImporter::~TextureImporter()
{}

const UID TextureImporter::Import(const std::string& file, const TextureImportConfiguration* config)
{
	UID uid;

	std::string path(App->file_system->GetAssets());
	path += "\\";
	path += file;

	char* buffer = nullptr;
	unsigned int length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer == nullptr || length == 0)
		return uid;

	ILuint imageID;				// Create an image ID as a ULuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, length); 	// Load the image file

	delete[] buffer;

	if (success)
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		//Config
		ilSetInteger(IL_DXTC_FORMAT, config->dxt_format);// To pick a specific DXT compression use

		if (config->format == TextureImportConfiguration::TF_TGA)
			ilSetInteger(IL_TGA_CREATE_STAMP, config->tga_stamp);
		if (config->format == TextureImportConfiguration::TF_JPG)
			ilSetInteger(IL_JPG_QUALITY, config->jpg_quality);
		if (config->format == TextureImportConfiguration::TF_PNG)
			ilSetInteger(IL_PNG_INTERLACE, config->interlaced);
		if (config->format == TextureImportConfiguration::TF_TGA)
			ilSetInteger(IL_TGA_RLE, config->rle);
		if (config->format == TextureImportConfiguration::TF_BMP)
			ilSetInteger(IL_BMP_RLE, config->rle);
		if (config->format == TextureImportConfiguration::TF_SGI)
			ilSetInteger(IL_SGI_RLE, config->rle);
		//--

		size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

		if (size > 0) {
			data = new ILubyte[size]; // allocate data buffer
			if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			{
				if (App->file_system->SaveFile((const char*)data, size, LIBRARY_TEXTURES_FOLDER, uid.GetAsName(), "dds") == false)
				{
					LOG("Could not save %s correctlly", file.c_str());
					return UID();
				}
				else
					LOG("Saved %s succesfully", file.c_str());
			}
			uid.Generate((char*)data, size);
			DELETE_ARRAY(data);
		}
	}
	return uid;
}

TextureSource* TextureImporter::Load(const UID& uid, const TextureLoadConfiguration* config)
{
	TextureSource* new_texture = nullptr;

	std::string path(App->file_system->GetTextures());
	path += "\\";
	path += uid.uid;
	path += ".dds";

	ILuint imageID;				// Create an image ID as a ULuint

	GLuint texture_id;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoad(IL_DDS, path.c_str()); 	// Load the image file

	if (success)	// If we managed to load the image, then we can start to do things with it...
	{
		//Config
		if (config->offset_x != 0)
			ilSetInteger(IL_IMAGE_OFFX, config->offset_x);
		if (config->offset_y != 0)
			ilSetInteger(IL_IMAGE_OFFY, config->offset_y);
		if (config->alienify)
			iluAlienify();
		if (config->blur_average)
			iluBlurAvg(config->blur_iterations);
		if (config->blur_gaussian)
			iluBlurGaussian(config->blur_iterations);
		if (config->contrast)
			iluContrast(config->amount_of_contrast);
		if (config->equalize)
			iluEqualize();
		if(config->gamma_correction)
			iluGammaCorrect(config->gamma_correction_factor);
		if (config->negative)
			iluNegative();
		if (config->noise)
			iluNoisify(config->noise_tolerance);
		if (config->pixelize)
			iluPixelize(config->pixels_across);
		if (config->sharpen)
			iluSharpen(config->sharpening_factor, config->sharpen_iterations);
		if (config->scaling)
		{
			iluImageParameter(ILU_FILTER, config->scaling_filter);
			iluScale(config->scale_width, config->scale_height, config->scale_depth);
		}
		//--

		new_texture = new TextureSource;
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		new_texture->SetDimensions(ImageInfo.Width, ImageInfo.Height);

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion

		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL reports error:%i - %s", error, iluErrorString(error));
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &texture_id);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, texture_id);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (config->mip_mapping)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Anysotropy
		if (config->anysotropy)
		{
			GLfloat ansio_level;

			if (config->max_anysotropy)
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ansio_level);
			else
				ansio_level = config->anysotropy_level;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ansio_level);
		}

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself

		if(config->mip_mapping)	
			glGenerateMipmap(GL_TEXTURE_2D);//THIS SHOULD ABSOLUTELLY NOT WORK AT ALL!!!!!! IT IS NOT SUPORTED UNTIL GL VERSION 3.1

		glBindTexture(GL_TEXTURE_2D, 0);

		//Fill our texture class
		new_texture->SetGLTextureType(GL_TEXTURE_2D);
		new_texture->SetTextureID(texture_id);

		LOG("Texture creation successful.");

		ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

		return new_texture;
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG("Image load failed - IL reports error:%i - %s", error, iluErrorString(error));
		return nullptr;
	}
}

bool TextureImporter::GenerateImage(const std::string & path, const TextureImportConfiguration * import_config, const TextureLoadConfiguration * load_config, GLuint& id, unsigned int& width, unsigned int& heigth)
{
	char* buffer = nullptr;
	unsigned int length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer == nullptr || length == 0)
		return false;

	ILuint imageID;				// Create an image ID as a ULuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, length); 	// Load the image file

	if (success)
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		//Config
		ilSetInteger(IL_DXTC_FORMAT, import_config->dxt_format);// To pick a specific DXT compression use

		if (import_config->format == TextureImportConfiguration::TF_TGA)
			ilSetInteger(IL_TGA_CREATE_STAMP, import_config->tga_stamp);
		if (import_config->format == TextureImportConfiguration::TF_JPG)
			ilSetInteger(IL_JPG_QUALITY, import_config->jpg_quality);
		if (import_config->format == TextureImportConfiguration::TF_PNG)
			ilSetInteger(IL_PNG_INTERLACE, import_config->interlaced);
		if (import_config->format == TextureImportConfiguration::TF_TGA)
			ilSetInteger(IL_TGA_RLE, import_config->rle);
		if (import_config->format == TextureImportConfiguration::TF_BMP)
			ilSetInteger(IL_BMP_RLE, import_config->rle);
		if (import_config->format == TextureImportConfiguration::TF_SGI)
			ilSetInteger(IL_SGI_RLE, import_config->rle);
		//--

		//Config
		if (load_config->offset_x != 0)
			ilSetInteger(IL_IMAGE_OFFX, load_config->offset_x);
		if (load_config->offset_y != 0)
			ilSetInteger(IL_IMAGE_OFFY, load_config->offset_y);
		if (load_config->alienify)
			iluAlienify();
		if (load_config->blur_average)
			iluBlurAvg(load_config->blur_iterations);
		if (load_config->blur_gaussian)
			iluBlurGaussian(load_config->blur_iterations);
		if (load_config->contrast)
			iluContrast(load_config->amount_of_contrast);
		if (load_config->equalize)
			iluEqualize();
		if (load_config->gamma_correction)
			iluGammaCorrect(load_config->gamma_correction_factor);
		if (load_config->negative)
			iluNegative();
		if (load_config->noise)
			iluNoisify(load_config->noise_tolerance);
		if (load_config->pixelize)
			iluPixelize(load_config->pixels_across);
		if (load_config->sharpen)
			iluSharpen(load_config->sharpening_factor, load_config->sharpen_iterations);
		if (load_config->scaling)
		{
			iluImageParameter(ILU_FILTER, load_config->scaling_filter);
			iluScale(load_config->scale_width, load_config->scale_height, load_config->scale_depth);
		}
		//--

		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion

		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL reports error:%i - %s", error, iluErrorString(error));
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &id);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, id);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (load_config->mip_mapping)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Anysotropy
		if (load_config->anysotropy)
		{
			GLfloat ansio_level;

			if (load_config->max_anysotropy)
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ansio_level);
			else
				ansio_level = load_config->anysotropy_level;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ansio_level);
		}

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself

		if (load_config->mip_mapping)
			glGenerateMipmap(GL_TEXTURE_2D);//THIS SHOULD ABSOLUTELLY NOT WORK AT ALL!!!!!! IT IS NOT SUPORTED UNTIL GL VERSION 3.1

		glBindTexture(GL_TEXTURE_2D, 0);

		width = ilGetInteger(IL_IMAGE_WIDTH);
		heigth = ilGetInteger(IL_IMAGE_HEIGHT);

		ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

		delete[] buffer;
		return true;
	}
	delete[] buffer;
	return false;
}
