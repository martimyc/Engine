#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "Globals.h"

class Primitive
{
protected:
	GLuint vertex_id = 0;
	GLuint indices_id = 0;
	const uint num_vertices = 0;
	const uint num_indices = 0;
	GLfloat* vertices = nullptr;
	GLuint* indices = nullptr;

public:
	Primitive(const uint num_vertices, const uint num_indices);
	~Primitive();

	GLuint GetVertexId();
	GLuint GetIndicesId();
	const uint GetVerticesNum();
	const uint GetIndicesNum();
	GLfloat* GetVertices();
	GLuint* GetIndices();
};

#endif // _PRIMITIVE_H