#ifndef ASSET
#define ASSET

#include <vector>
#include <string>

struct UID;
class GameObject;
class Resource;

struct ImportConfiguration
{
	virtual void Config() = 0;
};

struct LoadConfiguration
{
	virtual void Config() = 0;
};

enum ASSET_TYPE
{
	AT_NO_TYPE = 0,
	AT_TEXTURE,
	AT_MESH,
	AT_MATERIAL,
	AT_SCENE
};

class Asset
{
private:
	ASSET_TYPE type;

protected:
	Resource* resource;
	const LoadConfiguration* config;
	std::vector<const GameObject*> instances;
	
public:

	Asset(ASSET_TYPE type, const  LoadConfiguration* config);
	Asset(ASSET_TYPE type, Resource* resource);

	~Asset();

	ASSET_TYPE GetType() const;
	const LoadConfiguration* GetConfig() const;

	const UID& GetUID() const;

	void Asset::SetResource(Resource * new_resource);

	Resource* GetResource() const;

	const std::string& GetName() const;

	void AddInstance(const GameObject* go);
};

//Scenes
struct SceneImportConfiguration : public ImportConfiguration
{

};

struct SceneLoadConfiguration : public LoadConfiguration
{

};

#endif // !ASSET

