#ifndef _LIGHT
#define _LIGHT

#include "Color.h"
#include "MathGeoLib\src\Math\float3.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	math::vec position;

	int ref;
	bool on;
};

#endif //_LIGHT