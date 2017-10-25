#ifndef MATERIAL_IMPORTER
#define MATERIAL_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"

class Material;

struct MaterialTextureInfo
{
	uint lenght;
	char* name;
};

class MaterialImporter
{
private:

public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const aiMaterial* material, const std::string& scene_path, const std::string& name);
	bool Load(const std::string& name, Material& new_material);
};

#endif // !MATERIAL_IMPORTER

