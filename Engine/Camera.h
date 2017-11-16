#ifndef CAMERA
#define CAMERA

#include "MathGeoLib\src\Geometry\Frustum.h"
#include "Component.h"

class Camera :	public Component
{
private:
	std::string name;
	Frustum frustum;
	bool frustum_culling = true;
	std::vector<Plane> planes;

	int vertical_fov = 90;
	float near_plane_dist = 1.0f;
	float far_plane_dist = 15.0f;

	void RecalculateFOV();
	void TransformCamera();
	void ResetFrustumPlanes();


public:
	Camera(const std::string& name, bool enabled = true);
	~Camera();

	bool Start();

	bool Update();

	void Inspector();

	bool IsFrustumActive();
	bool FrustumCulling(const GameObject* root);
	void DrawFrustum();

	const std::string& GetName() const;
};

#endif // !CAMERA