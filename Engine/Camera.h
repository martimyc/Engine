#ifndef CAMERA
#define CAMERA

#include "MathGeoLib\src\Geometry\Frustum.h"
#include "Component.h"

class Camera :	public Component
{
private:
	Frustum frustum;
	bool frustum_culling = true;

public:
	Camera(const char* name, bool enabled = true);
	~Camera();

	bool Update();

	void Inspector();

};

#endif // !CAMERA