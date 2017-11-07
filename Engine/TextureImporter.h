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

	bool Import(std::string& file);
	Texture* Load(const std::string& name);
	Texture* LoadCheckers();
};

#endif // !TEXTURE_IMPORTER
