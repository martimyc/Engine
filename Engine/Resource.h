#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <vector>
#include "UID.h"

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
	UID id;

public:
	Resource(RESOURCE_TYPES type, const char* name, const UID& id): type(type), name(name), id(id)
	{}

	Resource(RESOURCE_TYPES type, const std::string& name, const UID& id) : type(type), name(name), id(id)
	{}

	Resource(RESOURCE_TYPES type, const char* name) : type(type), name(name)
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

	const UID& GetUID() const
	{
		return id;
	}

	void SetUID(const UID& new_id)
	{
		id = new_id;
	}
};

#endif // !RESOURCE



