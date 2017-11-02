#ifndef _COMPONENT
#define _COMPONENT

#include <string>

enum COMPONENT_TYPE
{
	CT_NO_TYPE = 0,
	CT_MESH_FILTER,
	CT_APPLIED_MATERIAL,
	CT_CAMERA
};

class Component
{
private:
	COMPONENT_TYPE type;
	bool enabled;

protected:
	std::string name;

public:	
	Component(COMPONENT_TYPE type, const char* name, bool enabled = true): type(type), enabled(enabled), name(name)
	{}
	Component(COMPONENT_TYPE type, const std::string& name, bool enabled = true) : type(type), enabled(enabled), name(name)
	{}
	~Component()
	{}

	void Enable()
	{
		enabled = true;
	}

	bool Enabled() const
	{
		return enabled;
	}

	virtual bool Update()
	{
		return true;
	}

	bool Disable()
	{
		enabled = false;
	}

	//Get
	const COMPONENT_TYPE GetType() const
	{
		return type;
	}

	const std::string& GetName() const
	{
		return name;
	}

	virtual void Inspector()
	{}

};

#endif // !_COMPONENT

