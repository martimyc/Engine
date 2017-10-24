#ifndef _TRANSFORMATION
#define _TRANSFORMATION

#include "SDL2\include\SDL.h"
#include "Assimp\include\vector3.h"
#include "Assimp\include\quaternion.h"
#include "Component.h"

/*namespace Assimp
{
	class aiMatrix4x4t;
	typedef aiMatrix4x4t aiMatrix4x4;
}
*/
#include "Assimp\include\matrix4x4.h"


class Transform : public Component
{
private:
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;
	float roll, pitch, yaw;	// roll (x-axis rotation)	// pitch (y-axis rotation)	// yaw (z-axis rotation)
	
	aiMatrix4x4* matrix;
	
	void Quat2Euler(const aiQuaternion q, float &roll, float &pitch, float &yaw);
	void Euler2Quat(const float roll, const float pitch, const float yaw, aiQuaternion& q);

public:
	Transform(const char* const name, bool enabled = true);
	~Transform();

	void TranslateRotateScalate();
	void ResetTranslateRotateScalate();

	void Inspector(int num_component);
};

#endif // !_TRANSFORMATION