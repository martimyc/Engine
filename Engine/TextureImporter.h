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
	bool Load(const std::string& name, Texture& new_texture);
	void LoadCheckers(Texture& new_texture);
};

#endif // !TEXTURE_IMPORTER
