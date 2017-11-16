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
struct UID;

struct TextureImporter
{
	TextureImporter();
	~TextureImporter();

	const UID Import(const std::string& file, const TextureImportConfiguration* config);
	void Load(Texture* to_load, const TextureLoadConfiguration* config);

	bool GenerateImage(const std::string& path, const TextureImportConfiguration* import_config, const TextureLoadConfiguration* load_config, GLuint& id, unsigned int& width, unsigned int& heigth);
};

#endif // !TEXTURE_IMPORTER
