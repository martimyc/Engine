#ifndef _COMPONENT
#define _COMPONENT

enum COMPONENT_TYPE
{
	CT_NO_TYPE = 0,
	CT_MESH,
	CT_MATERIAL
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

	virtual void Draw() const
	{}
};

#endif // !_COMPONENT

