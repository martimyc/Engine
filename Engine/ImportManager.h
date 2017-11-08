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

class Resource;
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

class ImportManager : public Module
{
private:
	std::string path;
	IMPORT_TYPE import_type = IT_NO_TYPE;
	bool importing = false;
	
	MeshImporter* mesh_importer = nullptr;
	MaterialImporter* material_importer = nullptr;
	TextureImporter* texture_importer = nullptr;

	friend MaterialImporter;

public:
	ImportManager(const char* name, bool start_enabled = true);
	~ImportManager();

private:
	bool Import(const std::string& path, IMPORT_TYPE type) const; //should return uid / 0 if it fails
	Texture* LoadTexture(const std::string& name) const;
	Material* LoadMaterial(const std::string& name) const;
	Mesh* LoadMesh(const std::string& name) const;
	//Scene* LoadScene(const std::string& name) const;

	//Scene
	bool ImportScene(const std::string& path) const;
	//bool LoadScene(const std::string& name) const;

	//Objects
	bool ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<Material*>& materials, bool* material_loads, const std::vector<Mesh*>& meshes, bool* mesh_loads) const;

	void ImportConfig();

public:
	bool Init();
	bool CleanUp();

	bool ImportAndLoad(const std::string& path, IMPORT_TYPE type);
};


#endif //!MODULE_IMPORTER
