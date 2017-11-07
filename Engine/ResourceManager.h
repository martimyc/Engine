#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include <vector>
#include "Module.h"

class Resource;
class Material;
class Texture;
class Mesh;

class ResourceManager: public Module
{
private:
	std::vector<Resource*> resources;

public:
	ResourceManager(const char * name, bool start_enabled);
	~ResourceManager();

	//Materials
	Material* CreateMaterial(const char* const name = nullptr);
	Material* GetMaterial(unsigned int pos) const;
	Material* GetMaterial(const std::string& name) const;
	void DeleteMaterial(Material* material_to_delete);
	bool HasMaterials() const;

	void ApplyToMaterial(Texture* new_text, int material);
	unsigned int GetNumMaterials() const;

	//Meshes
	Mesh* CreateMesh(const char* const name = nullptr);
	Mesh* GetMesh(unsigned int pos) const;
	Mesh* GetMesh(const std::string& name) const;
	void DeleteMesh(Mesh* mesh_to_delete);

	//Textures
	Texture* CreateTexture(const std::string& name, const TEXTURE_TYPE type = (TEXTURE_TYPE)1, const GLenum dimensions = GL_TEXTURE_2D, const GLuint& id = 0); //(TEXTURE_TYPE)1 == TT_DIFFUSE wich can not be forward declared
	Texture* GetTexture(unsigned int i) const;
	Texture* GetTexture(const std::string& name) const;
	void DeleteTexture(Texture* texture_to_delete);
	void AddTexture(Texture* new_texture);
	const int GetTextureToDraw() const;
	bool DebugTextures() const;
	bool TextureExsists(const std::string& path) const;
	void DrawTexture(unsigned int num_texture) const;
	unsigned int GetNumTextures() const;

};

#endif // !RESOURCE_MANAGER


