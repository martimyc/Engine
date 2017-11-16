#ifndef MATERIAL_IMPORTER
#define MATERIAL_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"

struct MaterialImportConfiguration;
struct MaterialLoadConfiguration;
struct Material;
struct UID;

struct MaterialTextureInfo
{
	unsigned int lenght;
	char* name;
};

class MaterialImporter
{
private:
	unsigned int GetTotalSize(const aiMaterial * material) const;

public:
	MaterialImporter();
	~MaterialImporter();

	const UID Import(const aiMaterial* material, const MaterialImportConfiguration* config);
	void Load(Material* to_load, unsigned int priority, const MaterialLoadConfiguration* config);
};

#endif // !MATERIAL_IMPORTER

