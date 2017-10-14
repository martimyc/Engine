#ifndef _CUBE_PRIMITIVE_H
#define _CUBE_PRIMITIVE_H

#include "Globals.h"
#include "Primitive.h"

class CubePrimitive : public Primitive
{
private:
	AABB* cube;

public:
	CubePrimitive();
	~CubePrimitive();

	bool LoadCube();
};

/*
	GLfloat vertices[24] =
	{
	0.f, 0.f, 0.f,		//0
	1.f, 0.f, 0.f,		//1
	1.f, 1.f, 0.f,		//2
	0.f, 1.f, 0.f,		//3
	0.f, 0.f, 1.f,		//4
	1.f, 0.f, 1.f,		//5
	1.f, 1.f, 1.f,		//6
	0.f, 1.f, 1.f		//7
	};

	GLuint indices[36] =
	{
	6,7,5, 7,4,5,	//Front
	6,5,1, 2,6,1,	//Right
	3,2,1, 0,3,1,	//Back
	7,3,0, 7,0,4,	//Left
	4,1,5, 4,0,1,	//Bottom
	3,6,2, 3,7,6	//Top
	};
*/
#endif // _CUBE_PRIMITIVE_H