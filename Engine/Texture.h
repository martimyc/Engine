#ifndef _TEXTURE
#define _TEXTURE

#include <string>
#include "glew\include\GL\glew.h"

enum TEXTURE_TYPES
{
	TT_NO_TYPE = 0
};

enum TEXTURE_DIMENSIONS
{
	TD_NO_DIMENSIONS = 0,
	TD_2D,
	TD_3D
};

struct Texture
{
	std::string path;
	TEXTURE_TYPES type;
	TEXTURE_DIMENSIONS dimensions;
	GLuint id;

	Texture(const char* path, const TEXTURE_TYPES type, const TEXTURE_DIMENSIONS dimensions, const GLuint& id): path(path), type(type), dimensions(dimensions), id(id)
	{}

	~Texture()
	{
		if(id != 0)
			glDeleteTextures(1, &id);
	}

	void SetUp()
	{

	}
};
#endif // !_TEXTURE

