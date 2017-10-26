#ifndef ASSET
#define ASSET

#include <string>
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

public:
	Asset(ASSET_TYPES type, const char* name): type(type), name(name)
	{}
	Asset(ASSET_TYPES type, const std::string& name) : type(type), name(name)
	{}

	~Asset()
	{}
	
	const std::string& GetName() const
	{
		return name;
	}
};

#endif // !ASSET



