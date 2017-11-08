#ifndef MATERIAL_IMPORTER
#define MATERIAL_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "UID.h"

class Material;

struct MaterialTextureInfo
{
	unsigned int lenght;
	char* name;
};

class MaterialImporter
{
private:
	unsigned int GetTotalSize(const aiMaterial * material, const std::string& scene_path) const;

public:
	MaterialImporter();
	~MaterialImporter();

	const UID Import(const aiMaterial* material, const std::string& scene_path, const std::string& name);
	Material* Load(const UID& id);
};

#endif // !MATERIAL_IMPORTER

