#ifndef _COMPONENT
#define _COMPONENT

enum COMPONENT_TYPE
{
	C_NO_TYPE = 0,
	C_MESH
};

class Component
{
private:
	COMPONENT_TYPE type;
	bool enabled;
public:
	Component(COMPONENT_TYPE type, bool enabled = true): type(type), enabled(enabled)
	{}
	~Component()
	{}

	void Enable()
	{
		enabled = true;
	}

	virtual bool Update() = 0;

	bool Disable()
	{
		enabled = false;
	}

	const COMPONENT_TYPE GetType() const
	{
		return type;
	}
};

#endif // !_COMPONENT

