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
struct TextureImportConfiguration;
struct TextureLoadConfiguration;
struct Material;
struct UID;

class MaterialImporter
{
private:
	unsigned int GetTotalSize(const aiMaterial * material) const;

	void ImportDiffuse(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportSpecular(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportAmbient(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportEmissive(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportHeight(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportNormals(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportShininess(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportOpacity(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportDisplacement(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportLightMap(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportReflection(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;
	void ImportUnknown(const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config) const;

	void LoadDiffuse(Material* to_load, char** iterator) const;
	void LoadSpecular(Material* to_load, char** iterator) const;
	void LoadAmbient(Material* to_load, char** iterator) const;
	void LoadEmissive(Material* to_load, char** iterator) const;
	void LoadHeight(Material* to_load, char** iterator) const;
	void LoadNormals(Material* to_load, char** iterator) const;
	void LoadShininess(Material* to_load, char** iterator) const;
	void LoadOpacity(Material* to_load, char** iterator) const;
	void LoadDisplacement(Material* to_load, char** iterator) const;
	void LoadLightMap(Material* to_load, char** iterator) const;
	void LoadReflection(Material* to_load, char** iterator) const;

public:
	MaterialImporter();
	~MaterialImporter();

	const UID Import(const aiMaterial* material, const MaterialImportConfiguration* config) const;
	bool Load(Material* to_load, unsigned int priority, const MaterialLoadConfiguration* config) const;
};

#endif // !MATERIAL_IMPORTER

