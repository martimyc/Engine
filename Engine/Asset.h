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
	virtual void MetaSave() const = 0;
	virtual void MetaLoad(char* buffer) = 0;
	virtual unsigned int GetMetaSize() const = 0;
};

struct LoadConfiguration
{
	virtual void Config() = 0;
	virtual void MetaSave() const = 0;
	virtual void MetaLoad(char* buffer) = 0;
	virtual unsigned int GetMetaSize() const = 0;
};

enum RESOURCE_TYPE;

class Asset
{
private:
	RESOURCE_TYPE type;

protected:
	Resource* resource;
	const ImportConfiguration* import_config;
	const LoadConfiguration* load_config;
	std::vector<const GameObject*> instances;
	
public:

	Asset(RESOURCE_TYPE type, Resource* resource, const ImportConfiguration* import_config, const  LoadConfiguration* load_config);
	Asset(RESOURCE_TYPE type, Resource* resource);

	~Asset();

	RESOURCE_TYPE GetType() const;

	const ImportConfiguration* GetImportConfig() const;
	const LoadConfiguration* GetLoadConfig() const;

	const UID& GetUID() const;

	void Asset::SetResource(Resource* new_resource);

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

