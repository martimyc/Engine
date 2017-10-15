#include "Globals.h"
#include "Material.h"
#include "Texture.h"

Material::Material()
{}

Material::~Material() //Deleting a material does not delete its textures
{
	for (std::vector<TextureWithUVs*>::iterator it = textures.begin(); it != textures.end(); ++it)
		delete (*it);
	textures.clear();
}

void Material::AssignTexturePointers(GLuint num_texture)
{
	glBindTexture(GL_TEXTURE_2D, textures[num_texture]->texture->id);
}

const int Material::NumTextures() const
{
	return textures.size();
}

void Material::AddTexture(Texture* new_text, const GLuint& uv_channel)
{
	textures.push_back(new TextureWithUVs(new_text, uv_channel));
	num_difusse_textures++;
}

void Material::Empty()
{
	textures.clear();
	num_difusse_textures = 0;
}

const GLuint Material::GetTextureCoordinateChannel(GLuint num_texture)
{
	return textures[num_texture]->uv_channel;
}

