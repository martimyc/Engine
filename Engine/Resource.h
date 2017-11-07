#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <vector>

class GameObject;

enum RESOURCE_TYPES
{
	RT_NO_TYPE = 0,
	RT_TEXTURE,
	RT_MESH,
	RT_MATERIAL
};

class Resource
{
protected:
	std::string name;
	RESOURCE_TYPES type;

public:
	Resource(RESOURCE_TYPES type, const char* name): type(type), name(name)
	{}
	Resource(RESOURCE_TYPES type, const std::string& name) : type(type), name(name)
	{}

	~Resource()
	{}

	virtual bool Inspector() = 0; //False == delete

	//Gets
	const std::string& GetName() const
	{
		return name;
	}

	RESOURCE_TYPES GetType() const
	{
		return type;
	}
};

#endif // !RESOURCE



