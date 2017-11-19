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

	void ImportDiffuse(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportSpecular(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportAmbient(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportEmissive(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportHeight(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportNormals(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportShininess(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportOpacity(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportDisplacement(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportLightMap(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportReflection(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;
	void ImportUnknown(const std::string& scene_dir, const aiMaterial* material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const;

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

	const UID Import(const std::string& scene_path, const aiMaterial* material, const MaterialImportConfiguration* config) const;
	bool Load(Material* to_load, unsigned int priority, const MaterialLoadConfiguration* config) const;
};

#endif // !MATERIAL_IMPORTER

