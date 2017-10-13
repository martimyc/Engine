#ifndef CUBE_PRIMITIVE_H
#define CUBE_PRIMITIVE_H

#include "Globals.h"

struct CubePrimitive
{
	GLuint vertex_id = 0;
	GLuint indices_id = 0;
	const uint num_vertices = 8 * 3;
	const uint num_indices = 36;
	float vertices[24] = {
		0.f, 0.f, 0.f,		//0
		1.f, 0.f, 0.f,		//1
		1.f, 1.f, 0.f,		//2
		0.f, 1.f, 0.f,		//3
		0.f, 0.f, 1.f,		//4
		1.f, 0.f, 1.f,		//5
		1.f, 1.f, 1.f,		//6
		0.f, 1.f, 1.f		//7
	};

	uint indices[36] =
	{
		6,7,5, 7,4,5,	//Front
		6,5,1, 2,6,1,	//Right
		3,2,1, 0,3,1,	//Back
		7,3,0, 7,0,4,	//Left
		4,1,5, 4,0,1,	//Bottom
		3,6,2, 3,7,6	//Top	
	};

	CubePrimitive();
};

#endif // CUBE_PRIMITIVE_H