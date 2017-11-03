#ifndef TRANSFORM
#define TRANSFORM

#include "SDL2\include\SDL.h"
#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float4x4.h"

class Transform
{
private:
	std::string name;
	float3 translation;
	float3 scaling;
	Quat rotation;
	float roll, pitch, yaw;	// roll (x-axis rotation)	// pitch (y-axis rotation)	// yaw (z-axis rotation)
	
	float4x4 transform_matrix;
	bool transform = false;

	void Quat2Euler(const Quat q, float &roll, float &pitch, float &yaw);
	void Euler2Quat(const float roll, const float pitch, const float yaw, Quat& q);

public:
	Transform(const char* name = "Transform");
	~Transform();

	const float4x4* GetTransformMatrix();
	const float3* GetTransformTranslation();
	const float3* GetTransformScale();
	const Quat* GetTransformRotation();

	bool Update();
	void Inspector();
};

#endif // !_TRANSFORM