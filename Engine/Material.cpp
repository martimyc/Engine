#include "Globals.h"
#include "Material.h"

Material::Material()
{}

Material::~Material()
{
	for (std::vector<GLuint>::iterator it = texture_ids.begin(); it != texture_ids.end(); ++it)
		glDeleteTextures(1, &(*it));
}

void Material::AssignDrawPointers()
{
	GLint max_texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

	for (int i = 0; i < texture_ids.size(); i++)
	{
		if (i > max_texture_units)
		{
			LOG("Can not render more than %i textures with this hardware, remaining textures will not be rendered", max_texture_units);
			break;
		}
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
	}
}

const int Material::NumTextures() const
{
	return texture_ids.size();
}
