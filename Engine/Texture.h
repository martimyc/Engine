#ifndef _TEXTURE
#define _TEXTURE

#include <string>
#include "glew\include\GL\glew.h"

enum TEXTURE_TYPES
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

struct Texture
{
	std::string path;
	TEXTURE_TYPES type;
	GLenum dimensions;
	GLuint id;

	Texture(const std::string& path, const TEXTURE_TYPES type, const GLenum dimensions, const GLuint& id): path(path), type(type), dimensions(dimensions), id(id)
	{}

	~Texture()
	{
		if(id != 0)
			glDeleteTextures(1, &id);
	}
};
#endif // !_TEXTURE

