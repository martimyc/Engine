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
	GLuint cube_vertex_id	 = 0;
	GLuint cube_indices_id	 = 0;
	const uint cube_num_vertices = 8 * 3;
	const uint cube_num_indices	 = 36;
	float cube_vertices[24] = {
		0.f, 0.f, 0.f,		//0
		1.f, 0.f, 0.f,		//1
		1.f, 1.f, 0.f,		//2
		0.f, 1.f, 0.f,		//3
		0.f, 0.f, 1.f,		//4
		1.f, 0.f, 1.f,		//5
		1.f, 1.f, 1.f,		//6
		0.f, 1.f, 1.f		//7
	};

	uint cube_indices[36] =
	{
		6,7,5, 7,4,5,	//Front
		6,5,1, 2,6,1,	//Right
		3,2,1, 0,3,1,	//Back
		7,3,0, 7,0,4,	//Left
		4,1,5, 4,0,1,	//Bottom
		3,6,2, 3,7,6	//Top	
	};

		//Plane

public:
	BasicPrimitives(const char* name, bool start_enabled = true);
	~BasicPrimitives();

	bool Init();

	bool LoadPrimitives();
	bool GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, float* vertices, uint& indices_id, uint& indices_num, uint* indices);
};

#endif // BASIC_PRIMITIVES_H