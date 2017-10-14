#ifndef _SPHERE_PRIMITIVE_H
#define _SPHERE_PRIMITIVE_H

#include "Globals.h"
#include "Primitive.h"

#define SPHERE_NUM_VERTICES 6144	//Must be  24 * 4 ^ n, where n >= 0. i.e: 24, 96, 384, 1536, 6144, 24576, ...

class SpherePrimitive : public Primitive
{
private:
	Sphere* sphere;

public:
	SpherePrimitive();
	~SpherePrimitive();

	bool LoadSphere();
};

#endif // _SPHERE_PRIMITIVE_H