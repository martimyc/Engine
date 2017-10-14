#include "glew\include\GL\glew.h"
#include "Primitive.h"

Primitive::Primitive(const uint num_vertices, const uint num_indices) : num_vertices(num_vertices), num_indices(num_indices)
{
	vertices = new GLfloat[num_vertices];
	indices = new GLuint[num_indices];
}

Primitive::~Primitive()
{
	DELETE_ARRAY(vertices);
	DELETE_ARRAY(indices);
}

GLuint Primitive::GetVertexId()
{
	return vertex_id;
}

GLuint Primitive::GetIndicesId()
{
	return indices_id;
}

const uint Primitive::GetVerticesNum()
{
	return num_vertices;
}

const uint Primitive::GetIndicesNum()
{
	return num_indices;
}

GLfloat * Primitive::GetVertices()
{
	return vertices;
}

GLuint * Primitive::GetIndices()
{
	return indices;
}
