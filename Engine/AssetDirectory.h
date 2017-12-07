#ifndef ASSET_DIRECTORY
#define ASSET_DIRECTORY

#include <string>
#include <vector>

typedef unsigned int GLuint;
class Asset;

class UID;
enum RESOURCE_TYPE;
struct ImportConfiguration;
struct LoadConfiguration;

class Material;
class Texture;
class Mesh;
class Prefab;

class GameObject;

class AssetDirectory
{
private:
	static GLuint icon;
	std::string name;
	std::vector<Asset*> assets;
	std::vector<AssetDirectory*> directories;

public:
	AssetDirectory(const std::string& name);
	~AssetDirectory();

	static void SetImage(GLuint id);

	const std::string& GetName() const;

	void AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config);
	void AddDir(AssetDirectory* new_directory);

	void RemoveAsset(Asset* to_remove);
	void RemoveDir(AssetDirectory* to_remove);

	void Inspector(unsigned int& selected);
	bool Hirarchy(AssetDirectory* focused);

	Material* UseMaterial(const UID& id, const GameObject* go) const;
	Texture* UseTexture(const UID& id, const Material* material) const;
	Mesh* UseMesh(const UID& id, const GameObject* go) const;
	Prefab* UsePrefab(const UID& id, const GameObject* go) const;
};

#endif // !ASSET_DIRECTORY



