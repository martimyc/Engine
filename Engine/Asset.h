#ifndef ASSET
#define ASSET

#include <string>
#include <vector>

class GameObject;

enum ASSET_TYPES
{
	AT_NO_TYPE = 0,
	AT_TEXTURE,
	AT_MESH,
	AT_MATERIAL
};

class Asset
{
protected:
	std::string name;
	ASSET_TYPES type;
	std::vector<const GameObject*> instances;

public:
	Asset(ASSET_TYPES type, const char* name): type(type), name(name)
	{}
	Asset(ASSET_TYPES type, const std::string& name) : type(type), name(name)
	{}

	~Asset()
	{}

	virtual bool Inspector() = 0; //False == delete

	//Gets
	const std::string& GetName() const
	{
		return name;
	}

	ASSET_TYPES GetType() const
	{
		return type;
	}
};

#endif // !ASSET



