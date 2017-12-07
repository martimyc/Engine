#ifndef _MODULE_CAMERA
#define _MODULE_CAMERA

#include "MathGeoLib\src\MathGeoLibFwd.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Geometry\Frustum.h"
#include "MathGeoLib\src\Geometry\Plane.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "Module.h"
#include "Globals.h"

class GameObject;

class Camera3D : public Module
{
private :
	Frustum editor_camera_frustum;
	float near_plane_dist_editor_camera = 0.5f;
	float far_plane_dist_editor_camera = 500.0f;
	math::vec pos;
	math::Quat rotation;
	float vertical_fov_editor_camera;
	std::vector<Plane> frustum_planes;

	bool config_camera = false;

	float camera_speed = 5.0f;
	float camera_zoom_speed = 1.75f;
	float sensibility = 0.25f;

	bool show_matrix_debug = false;

	math::float4x4 view_matrix;

public:
	Camera3D(const char* name, bool start_enabled = true);
	~Camera3D();

private:
	void LookAt(const vec &spot, float distance);

	void ResetFrustumPlanes();
	void ShowMatrixDebug();

public:
	bool Init();
	bool Start();
	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS Update(float dt);
	bool CleanUp();

	void OpenCloseConfigCameraWindow();
	void RecalculateFOV();

	const float GetFovVertical()const;
	const float* GetProjMatrix()const;
	const float* GetProjMatrixTransposed()const;
	const float* GetViewProjMatrix()const;
	const float * GetViewProjMatrixTransposed() const; 
	const float4x4 GetViewMatrix()const;
	const float* GetWorldMatrix()const;
		
	
	void CenterToGameObject(const GameObject* game_object);
	void OpenCloseMatricesDebugWindow();

	bool DoFrustumCulling(const GameObject* game_obj);
	LineSegment picking_ray;
	void DrawPickingRay();
};

#endif //_MODULE_CAMERA