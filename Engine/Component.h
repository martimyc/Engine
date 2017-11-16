#ifndef COMPONENT
#define COMPONENT

#include <string>

class GameObject;

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
	GameObject* game_object;

public:	
	Component(COMPONENT_TYPE type, bool enabled = true) : type(type), enabled(enabled)
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

	void SetGameObject(GameObject* go);

	//Get
	const COMPONENT_TYPE GetType() const
	{
		return type;
	}

	virtual const std::string& GetName() const = 0;

	virtual void Inspector() = 0;
};

#endif // !COMPONENT

