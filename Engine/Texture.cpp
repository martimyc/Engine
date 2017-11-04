#include "imgui\imgui.h"
#include "Globals.h"
#include "Texture.h"

Texture::Texture(const char* name, const TEXTURE_TYPE texture_type, const GLenum gl_texure_type, const GLuint& id) : Asset(AT_TEXTURE, name), texture_type(texture_type), gl_texure_type(gl_texure_type), id(id)
{}

Texture::Texture(const std::string& name, const TEXTURE_TYPE texture_type, const GLenum gl_texure_type, const GLuint& id) : Asset(AT_TEXTURE, name), texture_type(texture_type), gl_texure_type(gl_texure_type), id(id)
{}

Texture::~Texture()
{
	if (id != 0)
		glDeleteTextures(1, &id);
}

const unsigned int & Texture::GetHeight() const
{
	return height;
}

const unsigned int & Texture::GetWidth() const
{
	return width;
}

const TEXTURE_TYPE & Texture::GetTextureType() const
{
	return texture_type;
}

const GLenum & Texture::GetGLTextureType() const
{
	return gl_texure_type;
}

const GLuint & Texture::GetID() const
{
	return id;
}

void Texture::SetDimensions(const unsigned int & width, const unsigned int & height)
{
	this->width = width;
	this->height = height;
}

void Texture::SetTextureType(const TEXTURE_TYPE & new_texture_type)
{
	texture_type = new_texture_type;
}

void Texture::SetGLTextureType(const GLenum & new_gl_texure_type)
{
	gl_texure_type = new_gl_texure_type;
}

void Texture::SetID(const GLuint & new_id)
{
	id = new_id;
}

bool Texture::Inspector()
{
	bool ret = true;

	ImVec2 size(50, 50);
	ImVec2 uv0(0, 1);
	ImVec2 uv1(1, 0);

	ImGui::Image((void*)id, size, uv0, uv1);
	ImGui::SameLine();
	ImGui::Text("Width: %i\nHeight: %i", width, height);

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting textures");
		ret = false;
	}

	return ret;
}
