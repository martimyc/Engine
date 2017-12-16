#include "imgui\imgui.h"
#include "Globals.h"
#include "Texture.h"

//TextureSource
TextureSource::TextureSource(const GLenum gl_texure_type, const GLuint & texture_id) : gl_texure_type(gl_texure_type), texture_id(texture_id)
{}

TextureSource::~TextureSource()
{
	if (texture_id != 0)
		glDeleteTextures(1, &texture_id);
}

void TextureSource::SetDimensions(const unsigned int & width, const unsigned int & height)
{
	this->width = width;
	this->height = height;
}

void TextureSource::SetGLTextureType(const GLenum & new_gl_texure_type)
{
	gl_texure_type = new_gl_texure_type;
}

void TextureSource::SetTextureID(const GLuint & new_id)
{
	texture_id = new_id;
}

Texture::Texture(const std::string& name, const UID& uid): Resource(RT_TEXTURE, name, uid), source(nullptr)
{}

Texture::Texture(const std::string & name, TextureSource * source): Resource(RT_TEXTURE, name), source(source)
{}

Texture::~Texture()
{
	if(source != nullptr)
		delete source;
}

const unsigned int & Texture::GetHeight() const
{
	if (source != nullptr)
		return source->height;
	else
		LOG("Asking for unloaded texture information");
}

const unsigned int & Texture::GetWidth() const
{
	if (source != nullptr)
		return source->width;
	else
		LOG("Asking for unloaded texture information");
}

const GLenum & Texture::GetGLTextureType() const
{
	if (source != nullptr)
		return source->gl_texure_type;
	else
		LOG("Asking for unloaded texture information");
}

const GLuint & Texture::GetTextureID() const
{
	if (source != nullptr)
		return source->texture_id;
	else
		LOG("Asking for unloaded texture information");
}

bool Texture::Inspector()
{
	bool ret = true;

	ImVec2 size(50, 50);
	ImVec2 uv0(0, 1);
	ImVec2 uv1(1, 0);

	ImGui::Image((void*)source->texture_id, size, uv0, uv1);
	ImGui::SameLine();
	ImGui::Text("Width: %i\nHeight: %i", source->width, source->height);

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting textures");
		ret = false;
	}

	return ret;
}

bool Texture::IsLoaded() const
{
	return source != nullptr;
}

void Texture::UnLoad()
{
	delete source;
	source = nullptr;
}

void Texture::SetSource(TextureSource* source)
{
	this->source = source;
}
