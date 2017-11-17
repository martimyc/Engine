#ifndef ASSET
#define ASSET

#include <vector>
#include <string>

struct UID;
class GameObject;
class Resource;

struct ImportConfiguration
{
	virtual bool Config() = 0;
	virtual void MetaSave(char* iterator) const = 0;
	virtual void MetaLoad(char* iterator) = 0;
	virtual unsigned int GetMetaSize() const = 0;
};

struct LoadConfiguration
{
	virtual bool Config() = 0;
	virtual void MetaSave(char* iterator) const = 0;
	virtual void MetaLoad(char* iterator) = 0;
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
};

struct SceneImportConfiguration : public ImportConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

struct SceneLoadConfiguration : public LoadConfiguration
{
	virtual bool Config();
	virtual void MetaSave(char* iterator) const;
	virtual void MetaLoad(char* iterator);
	virtual unsigned int GetMetaSize() const;
};

#endif // !ASSET

