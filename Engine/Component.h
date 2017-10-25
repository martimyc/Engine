#ifndef _COMPONENT
#define _COMPONENT

#include <string>

enum COMPONENT_TYPE
{
	CT_NO_TYPE = 0,
	CT_MESH,
	CT_MATERIAL,
	CT_TRANSFORMATION
};

class GameObject;

class Component
{
private:
	COMPONENT_TYPE type;
	bool enabled;

protected:
	std::string name;
	bool edit_name;

public:
	GameObject* game_object;
	
	Component(COMPONENT_TYPE type, const char* name, GameObject* game_object, bool enabled = true): type(type), enabled(enabled), name(name), game_object(game_object), edit_name(false)
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

	const COMPONENT_TYPE GetType() const
	{
		return type;
	}

	const std::string& GetName() const
	{
		return name;
	}

	virtual void Draw() const
	{}

	virtual void Inspector(int num_component)
	{}

};

#endif // !_COMPONENT

