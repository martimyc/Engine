#include "glew\include\GL\glew.h"

#include "UID.h"

//Resources
#include "Resource.h"
#include "Asset.h"
#include "Texture.h"
#include "TextureAsset.h"
#include "Material.h"


//Modules
#include "ImportManager.h"
#include "Application.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

ResourceManager::~ResourceManager()
{}

Texture * ResourceManager::GetTexture(const UID & uid)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetType() == RT_TEXTURE && uid == (*it)->GetUID())
			return (Texture*)(*it)->GetResource();
	return nullptr;
}

bool ResourceManager::Init()
{
	debug_textures = LoadCheckers();

	return true;
}

void ResourceManager::AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config)
{
	Resource* new_resource = nullptr;
	Asset* new_asset = nullptr;

	switch (type)
	{
	case RT_TEXTURE:
		new_resource = new Texture(name, uid);
		new_asset = new TextureAsset(new_resource, import_config, load_config);
		break;
	case RT_SCENE:
		//new_asset = new SceneAsset(load_config);
		break;
	default:
		LOG("Non standard import type");
		break;
	}
	assets.push_back(new_asset);

	/*if (new_asset->GetType() == AT_TEXTURE)
		//TODO import textures and generate images to display in assets menu (textures are loaded cus we use them as image)*/
}

void ResourceManager::DeleteAsset(Asset * to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (to_delete == *it)
			delete *it;
}

bool ResourceManager::Exsists(const UID & id) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
			return true;
	return false;
}

Resource * ResourceManager::Use(const UID & id, const GameObject * go) const
{
	//TODO will probablly work diferently
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Resource* resource = (*it)->GetResource();

			if (resource == nullptr)
				if (App->import_manager->Load(resource, (*it)->GetLoadConfig()) == false)
					LOG("Could not load source for resource %s correctly", (*it)->GetName().c_str());

			if ((*it)->GetType() == RT_MATERIAL)
				((Material*)(*it))->Use(go);

			(*it)->AddInstance(go);
			return resource;
		}
	return nullptr;
}

//Materials
/*void ResourceManager::CreateEmptyMaterial(const char * const name)
{
	Material* new_material;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Material %i", num_materials);
		assets.push_back(new Asset(AT_MATERIAL, new Material(++material_priority), new_name));
	}
	else
		assets.push_back(new Asset(AT_MATERIAL, new Material(++material_priority), name));

	num_materials++;
}

void ResourceManager::ApplyToMaterial(Texture * new_text, unsigned int num_material)
{
	Material* material = nullptr;
	unsigned int current_material = 0;

	for (std::vector<AssetWithInstances*>::iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if ((*it)->type == AT_MATERIAL)
		{
			if (num_material == current_material)
			{
				((Material*)(*it)->resource)->AddTexture(new_text);
				return;
			}
			current_material++;
		}
	}

	LOG("Can not apply texture to material number %i, material non exsistent", num_material);
}*/

Texture* ResourceManager::LoadCheckers()
{
	TextureSource* source = new TextureSource;

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Anysotropy
	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	source->SetTextureID(checkers_text_id);
	source->SetDimensions(CHECKERS_WIDTH, CHECKERS_HEIGHT);

	Texture* new_texture = new Texture("Checkers", source);
	assets.push_back(new Asset(RT_TEXTURE, new_texture));
	num_textures++;

	return new_texture;
}

unsigned int ResourceManager::GetNumMaterials()
{
	return num_materials;
}

void ResourceManager::DebugTextures() const
{
	if (debug_textures)
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, debug_texture->GetTextureID());

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
}