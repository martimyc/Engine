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
	float near_plane_dist = 0.5f;
	float far_plane_dist = 500.0f;

	void RecalculateFOV();
	void ResetFrustumPlanes();

public:
	Camera(const std::string& name, const GameObject* const game_object, bool enabled = true);
	Camera(const Camera& copy);
	~Camera();

	bool Start();

	bool Update();

	void Inspector();

	void SaveComponent(char** iterator)const;

	void TransformCamera();
	bool IsFrustumActive();
	bool FrustumCulling(const GameObject* root);
	void DrawFrustum();

	const std::string& GetName() const;
};

#endif // !CAMERA