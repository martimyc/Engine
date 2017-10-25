#ifndef _TRANSFORMATION
#define _TRANSFORMATION

#include "SDL2\include\SDL.h"
#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "Component.h"

class GameObject;

class Transform : public Component
{
private:
	float3 translation;
	float3 scaling;
	Quat rotation;
	float roll, pitch, yaw;	// roll (x-axis rotation)	// pitch (y-axis rotation)	// yaw (z-axis rotation)
	
	float4x4 transform_matrix;
	
	void Quat2Euler(const Quat q, float &roll, float &pitch, float &yaw);
	void Euler2Quat(const float roll, const float pitch, const float yaw, Quat& q);

public:
	Transform(const char* const name, GameObject* game_object, bool enabled = true);
	~Transform();

	const float* GetTransformMatrix();

	void Inspector(int num_component);
};

#endif // !_TRANSFORMATION