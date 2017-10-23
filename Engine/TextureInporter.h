#ifndef _MODULE_TEXTURE_MANAGER
#define _MODULE_TEXTURE_MANAGER

#include <string>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Module.h"

class Texture;

struct TextureInporter
{
	TextureInporter();
	~TextureInporter();

	bool Import(const std::string& path);
	bool LoadTexture(const std::string& path, Texture& new_texture);
};

#endif // !_MODULE_TEXTURE_MANAGER
