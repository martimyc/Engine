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

	const UID Import(const std::string& file, const TextureImportConfiguration* config);
	TextureSource* Load(const UID& uid, const TextureLoadConfiguration* config);

	bool GenerateImage(const std::string& path, const TextureImportConfiguration* import_config, const TextureLoadConfiguration* load_config, GLuint& id, unsigned int& width, unsigned int& heigth);
};

#endif // !TEXTURE_IMPORTER
