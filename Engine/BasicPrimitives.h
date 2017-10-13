#ifndef BASIC_PRIMITIVES_H
#define BASIC_PRIMITIVES_H

#include "Module.h"

enum PRIMITIVE_TYPE
{
	NULL_PRIMITIVE = 0,
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

class BasicPrimitives : public Module
{
private:
	//All vertex && index ID's from basic primitives
		//Cube
	const uint cube_vertex_id	 = 0;
	const uint cube_indices_id	 = 0;
	const uint cube_num_vertices = 8 * 3;
	const uint cube_num_indices	 = 36;

		//Plane

public:
	BasicPrimitives(const char* name, bool start_enabled = true);
	~BasicPrimitives();

	bool Init();

	bool LoadPrimitives();
	bool GetPrimitiveId(PRIMITIVE_TYPE primitive,  uint& vertex_id, uint& vertices_num, uint& indices_id, uint& indices_num);
};

#endif // BASIC_PRIMITIVES_H