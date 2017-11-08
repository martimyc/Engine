#ifndef TEXTURE_IMPORTER
#define TEXTURE_IMPORTER

#include <string>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

class Texture;

struct TextureImporter
{
	TextureImporter();
	~TextureImporter();

	bool Import(const std::string& file);
	Texture* Load(const std::string& name);
};

#endif // !TEXTURE_IMPORTER
