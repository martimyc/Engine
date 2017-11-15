#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <vector>
#include "UID.h"

enum RESOURCE_TYPE
{
	RT_NO_TYPE,
	RT_TEXTURE,
	RT_MESH,
	RT_MATERIAL,
	RT_PREFAB
};

class Resource
{
private:
	RESOURCE_TYPE type;

protected:
	UID uid;
	std::string name;

	Resource(RESOURCE_TYPE type, const std::string& name) : type(type), name(name)
	{}

public:
	Resource(RESOURCE_TYPE type, const std::string& name, const UID& uid): type(type), name(name), uid(uid)
	{}

	virtual bool Inspector() = 0; //False == delete

	virtual bool IsLoaded() const = 0;

	RESOURCE_TYPE GetType() const
	{
		return type;
	}

	const UID& GetUID() const
	{
		return uid;
	}

	const std::string& GetName() const
	{
		return name;
	}
};

#endif // !RESOURCE



