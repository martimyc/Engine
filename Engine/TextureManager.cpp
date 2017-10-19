#include "imgui\imgui.h"
#include "Application.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Console.h"
#include "Texture.h"
#include "TextureManager.h"

TextureManager::TextureManager(const char* name, bool start_enabled): Module(name, start_enabled)
{}

TextureManager::~TextureManager()
{
	delete checkers;
}

bool TextureManager::LoadTexture(const std::string& path, Texture& new_texture)
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
		glGenTextures(1, &texture_id);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, texture_id);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		if (OPENGL_CONTEXT_MAJOR_VERSION > 2)
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

		glBindTexture(GL_TEXTURE_2D, 0);

		//Fill our texture class
		new_texture.dimensions = GL_TEXTURE_2D;
		new_texture.id = texture_id;

		AddTexture(&new_texture);

		LOG("Texture creation successful.");
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG("Image load failed - IL reports error:%i - %s", error, iluErrorString(error));
		ret = false;
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	return ret;
}

Texture * TextureManager::CreateTexture(const std::string& path, const TEXTURE_TYPES type, const GLenum dimensions, const GLuint& id)
{
	Texture* new_texture = new Texture(path, type, dimensions, id);
	textures.push_back(new_texture);
	return new_texture;
}

bool TextureManager::Init()
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

		if (ImGui::Button("Debug Textures"))
			debug_textures = !debug_textures;

		if (debug_textures)
		{
			if (ImGui::InputInt("Texture to draw", &texture_to_draw))
			{
				if (texture_to_draw >= textures.size())
				{
					LOG("Texture %i does not exist, binding Checkers", texture_to_draw);
					texture_to_draw = 0;
				}
				else if (texture_to_draw < 0)
					texture_to_draw = 0;				
			}			
		}

		std::vector<int> textures_to_delete;

		for (int i = 0; i < textures.size(); i++)
		{
			char text_name[255];
			sprintf(text_name, "Texture: %i", i);

			if (ImGui::TreeNode(text_name))
			{
				ImGui::Text("Path: %s", textures[i]->path.c_str());

				if (ImGui::Button("Add to material"))
						App->scene_manager->ApplyToMaterial(textures[i], current_material);

				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					LOG("Deleting textures");
					textures_to_delete.push_back(i);
				}

				ImGui::Text("Current material:");
				if (ImGui::InputInt("", &current_material))
				{
					if (current_material < 0)
						current_material = 0;
					else if (current_material > App->scene_manager->NumMaterials())
					{
						LOG("Material %i does not exsist", current_material);
						current_material = App->scene_manager->NumMaterials();
					}
				}

				ImGui::TreePop();
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_delete.begin(); it != textures_to_delete.end(); ++it)
		{
			delete textures[*it];
			textures.erase(textures.begin() + (*it));
		}			
	}

	return UPDATE_CONTINUE;
}

void TextureManager::DrawTexture(unsigned int num_texture)
{
	if (num_texture < textures.size())
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, textures[num_texture]->id);

		float hsize = 6.0f; // Vertical size of the quad
		float vsize = 4.0f; // Vertical size of the quad

		// Draw our texture
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);

		// Top right
		glTexCoord2f(1.0, 0.0);
		glVertex3f(hsize, -vsize, 0.0f);

		// Bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex3f(hsize, vsize, 0.0f);

		// Bottom left
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-hsize, vsize, 0.0f);

		// Top left
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-hsize, -vsize, 0.0f);
		
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else
		LOG("Can't draw texture, last existing texture is %i", textures.size() - 1);
}

Texture* TextureManager::LoadTextureStraightFromPath(const std::string& path)
{
	if (!Exsists(path))
	{
		//Assume 2D difuse
		Texture* new_texture = CreateTexture(path, TT_DIFFUSE, GL_TEXTURE_2D, 0);

		if (!App->texture_manager->LoadTexture(path, *new_texture))
		{
			delete new_texture;
			LOG("Error loading texture '%s'\n", path.c_str());
			return nullptr;
		}
		else
			return new_texture;
	}
	else
		LOG("Texture already exsists");

	return nullptr;
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

Texture * TextureManager::GetSecondTexture()
{
	return textures[1];
}

const int TextureManager::GetTextureToDraw() const
{
	return texture_to_draw;
}

bool TextureManager::DebugTextures() const
{
	return debug_textures;
}

bool TextureManager::Exsists(const std::string& path) const
{
	std::string new_name;
	size_t start = path.find_last_of("//");
	size_t end = path.find_last_of(".");
	// make sure the poisition is valid
	if (start == path.length() || end == path.length())
		LOG("Coud not create texture name");
	else
		new_name = path.substr(start + 1, end);

	for (std::vector<Texture*>::const_iterator it = textures.begin(); it != textures.end(); ++it)
		if ((*it)->path == new_name)
			return true;
	return false;
}
