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

const UID& TextureImporter::Import(const std::string& file, const TextureImportConfiguration& config)
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

	if (success)
	{
		uid.Generate(buffer, length);

		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

		if (size > 0) {
			data = new ILubyte[size]; // allocate data buffer
			if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			{
				if (App->file_system->SaveFile((const char*)data, size, LIBRARY_TEXTURES_FOLDER, uid.uid, "dds") == false)
				{
					LOG("Could not save %s correctlly", file.c_str());
					return UID();
				}
				else
					LOG("Saved %s succesfully", file.c_str());
			}
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

		//THIS SHOULD ABSOLUTELLY NOT WORK AT ALL!!!!!! IT IS NOT SUPORTED UNTIL GL VERSION 3.1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Anysotropy
		if (config->anysotropy)
		{
			GLfloat ansio_level;

			if (config->max_anysotropy)
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ansio_level);
			else
				ansio_level = config->anysotropy_level;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

		//THIS SHOULD ABSOLUTELLY NOT WORK AT ALL!!!!!! IT IS NOT SUPORTED UNTIL GL VERSION 3.1
		glGenerateMipmap(GL_TEXTURE_2D);

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