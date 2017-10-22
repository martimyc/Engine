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
	std::string name;
	unsigned int height;
	unsigned int width;
	TEXTURE_TYPES type;
	GLenum dimensions;
	GLuint id;

	Texture(const std::string& path, const TEXTURE_TYPES type = TT_DIFFUSE, const GLenum dimensions = GL_TEXTURE_2D, const GLuint& id = 0): path(path), type(type), dimensions(dimensions), id(id)
	{
		size_t start = path.find_last_of("\\");
		size_t end = path.find_last_of(".");
		// make sure the poisition is valid
		if (start == path.length() || end == path.length())
			LOG("Coud not create texture name"); 
		else
		{
			if (path == "Checkers")
				name = "Checkers";
			else
				name = path.substr(start + 1, end);
		}
	}

	Texture(const Texture& copy): path(copy.path), type(copy.type), dimensions(copy.dimensions), id(copy.id)
	{}

	~Texture()
	{
		if(id != 0)
			glDeleteTextures(1, &id);
	}
};
#endif // !_TEXTURE

