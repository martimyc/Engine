#ifndef CUBE__H
#define CUBE__H

#include "Primitive.h"

class AABB;

class Cube : public Primitive
{
public:

	Cube();
	Cube(const Cube& copy);
	~Cube();

public:

	void Initialize()final;

protected:

	math::AABB* cube = nullptr;

};

#endif // CUBE__H