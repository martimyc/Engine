#ifndef COMPONENT
#define COMPONENT

#include <string>

class GameObject;

enum COMPONENT_TYPE
{
	CT_NO_TYPE = 0,
	CT_MESH_FILTER,
	CT_APPLIED_MATERIAL,
	CT_CAMERA,
	CT_ANIMATOR
};

class Component
{
private:
	COMPONENT_TYPE type;

protected:
	bool enabled;
	const GameObject* const game_object;

public:	
	Component(COMPONENT_TYPE type, const GameObject* const go, bool enabled = true) : type(type), game_object(go), enabled(enabled)
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

	virtual bool Start()
	{
		return true;
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

	virtual const std::string& GetName() const = 0;

	virtual void Inspector() = 0;

	virtual void SaveComponent(char** iterator)const = 0;

};

#endif // !COMPONENT

