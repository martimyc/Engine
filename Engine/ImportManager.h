#ifndef MODULE_IMPORTER
#define MODULE_IMPORTER

#include <string>
#include <vector>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"
#include "Module.h"

class MeshImporter;
class MaterialImporter;
class TextureImporter;

class Texture;
class Material;
class Mesh;

class GameObject;

enum IMPORT_TYPE
{
	IT_NO_TYPE = 0,
	IT_TEXTURE,
	IT_SCENE
};

enum LOAD_TYPE
{
	LT_NO_TYPE = 0,
	LT_TEXTURE,
	LT_SCENE,
	LT_MESH,
	LT_MATERIAL
};

class ImportManager : public Module
{
private:
	
	MeshImporter* mesh_importer = nullptr;
	MaterialImporter* material_importer = nullptr;
	TextureImporter* texture_importer = nullptr;

public:
	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

	bool Init();
	bool CleanUp();

	bool ImportFromPath(const std::string& path, IMPORT_TYPE type, void* imported = nullptr);
	bool Load(const std::string& name, LOAD_TYPE type, void* loaded = nullptr);

	//Scene
	bool ImportScene(const std::string& path) const;
	//bool LoadScene(const std::string& name) const;

	//Objects
	bool ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<Material*>& materials, bool* material_loads, const std::vector<Mesh*>& meshes, bool* mesh_loads) const;

	//Textures
	Texture* LoadTexture(const std::string& name) const;

	//Materials
	Material* LoadMaterial(const std::string& name) const;

	//Meshes
	Mesh* LoadMesh(const std::string& name) const;

	void LoadCheckers();
};


#endif //!MODULE_IMPORTER
