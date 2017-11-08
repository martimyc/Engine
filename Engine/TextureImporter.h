#ifndef TEXTURE_IMPORTER
#define TEXTURE_IMPORTER

#include <string>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

class Texture;
struct TextureImportConfiguration;
struct TextureLoadConfiguration;

struct TextureImporter
{
	TextureImporter();
	~TextureImporter();

	const UID Import(const std::string& file, const TextureImportConfiguration& config);
	Texture* Load(const UID& uid, const TextureLoadConfiguration& config);
};

#endif // !TEXTURE_IMPORTER
