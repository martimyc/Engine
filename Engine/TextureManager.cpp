#include "imgui\imgui.h"
#include "Application.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Console.h"
#include "Texture.h"
#include "TextureManager.h"

TextureManager::TextureManager(const char* name, bool start_enabled): Module(name, start_enabled)
{}

bool TextureManager::Init()
{
	ilInit();
	iluInit();

	//checkers texture
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	//Load Texture to VRAM
	GLuint checkers_text_id;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkers_text_id);
	glBindTexture(GL_TEXTURE_2D, checkers_text_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkers = new Texture(std::string("Checkers"), TT_DIFFUSE, GL_TEXTURE_2D, checkers_text_id);
	AddTexture(checkers);

	return true;
}

UPDATE_STATUS TextureManager::Configuration(float dt)
{
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::Button("Empty"))
			EmptyTextures();

		for (std::vector<Texture*>::const_iterator it = textures.begin(); it != textures.end(); ++it)
		{
			ImGui::Text("path: %s", (*it)->path.c_str());
		}
			
	}

	return UPDATE_CONTINUE;
}

bool TextureManager::LoadTexture(const std::string& path, Texture& new_texture, bool hiest_quality)
{
	bool ret = true;

	ILuint imageID;				// Create an image ID as a ULuint

	GLuint texture_id;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(path.c_str()); 	// Load the image file

	if (success)	// If we managed to load the image, then we can start to do things with it...
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

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

	
		if (hiest_quality)
		{
			// Set texture interpolation method to the highest visual quality it can be:
			// GL_LINEAR_MIPMAP_LINEAR for minification - i.e. trilinear filtering
			// GL_LINEAR for magnification (choices are either GL_NEAREST or GL_LINEAR - we do not use any MIPMAP settings for magnification!!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			// Set texture interpolation method to use linear interpolation (no MIPMAPS)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

		//Fill our texture class
		new_texture.dimensions = GL_TEXTURE_2D;
		new_texture.id = texture_id;

		AddTexture(&new_texture);
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG("Image load failed - IL reports error:%i - %s", error, iluErrorString(error));
		ret = false;
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	LOG("Texture creation successful.");

	return ret;
}

void TextureManager::LoadTextureStraightFromPath(const std::string& path)
{
	//Assume 2D difuse
	Texture* new_texture = new Texture(path, TT_DIFFUSE, GL_TEXTURE_2D, 0);

	if (!App->texture_manager->LoadTexture(path, *new_texture))
	{
		delete new_texture;
		LOG("Error loading texture '%s'\n", path.c_str());
	}
	else
	{
		//only to test
		App->scene_manager->game_object->ApplyTexture(new_texture);
	}

}

void TextureManager::AddTexture(Texture * new_texture)
{
	textures.push_back(new_texture);
}

void TextureManager::EmptyTextures()
{
	for (std::vector<Texture*>::iterator it = textures.begin() + 1; it != textures.end(); ++it)
		delete (*it);
	textures.clear();
	textures.push_back(checkers);
}

Texture * TextureManager::GetCheckers()
{
	return textures[0];
}