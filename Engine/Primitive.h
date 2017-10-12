#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Globals.h"
#include "Color.h"
#include "MathGeoLib\src\MathGeoLib.h"

enum PRIMITIVE_TYPE
{
	UNDEF_PRIMITIVE = 0,
	PRIMITIVE_POINT,
	PRIMITIVE_LINE,
	PRIMITIVE_CUBE,
	PRIMITIVE_PLANE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_RAY,
	PRIMITIVE_CAPSULE,
	PRIMITIVE_FRUSTUM
};

struct SimpleMesh
{
	virtual ~SimpleMesh()
	{
		delete[]indices;
		delete[]vertices;
	}

	uint	id_indices = 0; // id in VRAM
	uint	num_indices = 0;
	uint*	indices = nullptr;

	uint	id_vertices = 0; // id in VRAM
	uint	num_vertices = 0;
	float*	vertices = nullptr;
};

class Primitive : public SimpleMesh
{
public:
	Primitive(PRIMITIVE_TYPE type);
	Primitive(const Primitive& copy);
	~Primitive();

private:
	PRIMITIVE_TYPE	type = UNDEF_PRIMITIVE;

public:
	virtual void	Initialize();
	virtual void	Draw();

};


#endif // PRIMITIVE_H