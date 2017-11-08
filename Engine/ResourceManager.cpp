#include "glew\include\GL\glew.h"

//Resources
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

//Modules
#include "ResourceManager.h"

ResourceManager::ResourceManager(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

ResourceManager::~ResourceManager()
{}

bool ResourceManager::Init()
{
	debug_texture = LoadCheckers();

	return true;
}

//Materials
void ResourceManager::AddMaterial(Material * new_material)
{
	num_materials++;
	assets.push_back(new Asset(new_material));
}

Material * ResourceManager::CreateEmptyMaterial(const char * const name)
{
	Material* new_material;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Material %i", num_materials);
		new_material = new Material(new_name, ++material_priority);
	}
	else
		new_material = new Material(name, ++material_priority);

	num_materials++;
	assets.push_back(new Asset(new_material));
	return new_material;
}

void ResourceManager::ApplyToMaterial(Texture * new_text, unsigned int num_material)
{
	Material* material = nullptr;
	unsigned int current_material = 0;

	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if ((*it)->resource->GetType() == RT_MATERIAL)
		{
			if (num_material == current_material)
			{
				((Material*)(*it)->resource)->AddTexture(new_text);
				return;
			}
			current_material++;
		}
	}

	LOG("Can not apply texture %s to material number %i, material non exsistent", new_text->GetName().c_str(), num_material);
}

void ResourceManager::DeleteMaterial(Material* material_to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->resource == material_to_delete)
		{
			delete *it;
			return;
		}
}

//Meshes
void ResourceManager::AddMesh(Mesh* new_mesh)
{
	num_meshes++;
	assets.push_back(new Asset(new_mesh));
}

void ResourceManager::DeleteMesh(Mesh * mesh_to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->resource == mesh_to_delete)
		{
			delete *it;
			return;
		}
}

//Textures
const Texture* ResourceManager::LoadCheckers()
{
	Texture* new_texture = new Texture("Checkers");

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

	new_texture->SetID(checkers_text_id);
	new_texture->SetDimensions(CHECKERS_WIDTH, CHECKERS_HEIGHT);

	assets.push_back(new Asset(new_texture));
	num_textures++;
	return new_texture;
}

void ResourceManager::AddTexture(Texture * new_texture)
{
	assets.push_back(new Asset(new_texture));
	num_textures++;
}

void ResourceManager::DeleteTexture(Texture * texture_to_delete)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->resource == texture_to_delete)
		{
			delete (*it);
			assets.erase(it);
			break;
		}
}

void ResourceManager::DebugTextures() const
{
	if (debug_textures)
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, debug_texture->GetID());

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

//Asset struct
Asset::Asset(Resource * resource) : resource(resource)
{}

Asset::~Asset()
{
	delete resource;

	instances.clear();
}
