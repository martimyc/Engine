#ifndef _TEXTURE
#define _TEXTURE

#include <string>
#include "glew\include\GL\glew.h"
#include "Asset.h"

enum TEXTURE_TYPE
{
	TT_NO_TYPE = 0,
	TT_DIFFUSE,
	TT_SPECULAR,
	TT_AMBIENT,
	TT_EMISSIVE,
	TT_HEIGHT,
	TT_NORMALS,
	TT_SHININESS,
	TT_OPACITY,
	TT_DISPLACEMENT,
	TT_LIGHTMAP,
	TT_REFLECTION,
	TT_UNKNOWN,
};

class Texture: public Asset
{
private:
	unsigned int height;
	unsigned int width;
	TEXTURE_TYPE texture_type;
	GLenum gl_texure_type;
	GLuint id;

public:
	Texture(const char* name, const TEXTURE_TYPE texture_type = TT_DIFFUSE, const GLenum gl_texure_type = GL_TEXTURE_2D, const GLuint& id = 0): Asset(AT_TEXTURE, name), texture_type(texture_type), gl_texure_type(gl_texure_type), id(id)
	{}

	Texture(const std::string& name, const TEXTURE_TYPE texture_type = TT_DIFFUSE, const GLenum gl_texure_type = GL_TEXTURE_2D, const GLuint& id = 0) : Asset(AT_TEXTURE, name), texture_type(texture_type), gl_texure_type(gl_texure_type), id(id)
	{}

	~Texture()
	{
		if(id != 0)
			glDeleteTextures(1, &id);
	}

	//Gets
	const unsigned int& GetHeight() const
	{
		return height;
	}

	const unsigned int& GetWidth() const
	{
		return width;
	}

	const TEXTURE_TYPE& GetTextureType() const
	{
		return texture_type;
	}

	const GLenum& GetGLTextureType() const
	{
		return gl_texure_type;
	}

	const GLuint& GetID() const
	{
		return id;
	}

	//Sets
	void SetDimensions(const unsigned int& width, const unsigned int& height)
	{
		this->width = width;
		this->height = height;
	}

	void SetTextureType(const TEXTURE_TYPE& new_texture_type)
	{
		texture_type = new_texture_type;
	}

	void SetGLTextureType(const GLenum& new_gl_texure_type)
	{
		gl_texure_type = new_gl_texure_type;
	}

	void SetID(const GLuint& new_id)
	{
		id = new_id;
	}
};
#endif // !_TEXTURE

