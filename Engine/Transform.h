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

	void Quat2Euler(const Quat q, float &roll, float &pitch, float &yaw);
	void Euler2Quat(const float roll, const float pitch, const float yaw, Quat& q);

public:
	Transform(const char* name = "Transform");
	~Transform();

	void SetTransform(const math::float4x4& new_transform);

	const float4x4& GetTransformMatrix();
	const vec& GetTransformTranslation();
	const vec& GetTransformScale();
	const Quat& GetTransformRotation();
	const vec& GetTransformRotationAngles();
	Quat TransformEuler2Quat(const vec euler);

	void Update();
	bool Inspector();
};

#endif // !_TRANSFORM