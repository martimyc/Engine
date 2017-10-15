#include "Globals.h"
#include "Material.h"
#include "Texture.h"

Material::Material()
{}

Material::~Material() //Deleting a material does not delete its textures
{}

void Material::AssignDrawPointers()
{
	/*GLint max_texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

	for (int i = 0; i < textures.size(); i++)
	{
		if (i > max_texture_units)
		{
			LOG("Can not render more than %i textures with this hardware, remaining textures will not be rendered", max_texture_units);
			break;
		}

		//textures[i]->SetUp();
		glActiveTexture(GL_TEXTURE0 + i);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i]->id);	
	}*/

	glBindTexture(GL_TEXTURE_2D, textures[0]->id);
}

const int Material::NumTextures() const
{
	return textures.size();
}

void Material::AddTexture(Texture* new_text)
{
	textures.push_back(new_text);
	num_difusse_textures++;
}

void Material::Empty()
{
	textures.clear();
	num_difusse_textures = 0;
}
