#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "Globals.h"

class Primitive
{
protected:
	GLuint vertex_id = 0;
	GLuint indices_id = 0;
	GLuint uvs_id = 0;
	const uint num_vertices = 0;
	const uint num_indices = 0;
	GLfloat* vertices = nullptr;
	GLuint* indices = nullptr;
	GLfloat* uvs = nullptr;

public:
	Primitive(const uint num_vertices, const uint num_indices);
	~Primitive();

	GLuint GetVertexId() const;
	GLuint GetIndicesId() const;
	GLuint GetUVsId() const;
	const uint GetVerticesNum() const;
	const uint GetIndicesNum() const;
	GLfloat* GetVertices() const;
	GLuint* GetIndices() const;
	GLfloat* GetUVs() const;
};

#endif // _PRIMITIVE_H