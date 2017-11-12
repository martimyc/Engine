#ifndef TEXTURE_IMPORTER
#define TEXTURE_IMPORTER

#include <string>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

class TextureSource;
struct TextureImportConfiguration;
struct TextureLoadConfiguration;
struct UID;

struct TextureImporter
{
	TextureImporter();
	~TextureImporter();

	const UID& Import(const std::string& file, const TextureImportConfiguration& config);
	TextureSource* Load(const UID& uid, const TextureLoadConfiguration* config);
};

#endif // !TEXTURE_IMPORTER
