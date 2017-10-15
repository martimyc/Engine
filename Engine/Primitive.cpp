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

GLuint Primitive::GetVertexId() const
{
	return vertex_id;
}

GLuint Primitive::GetIndicesId() const
{
	return indices_id;
}

const uint Primitive::GetVerticesNum() const
{
	return num_vertices;
}

const uint Primitive::GetIndicesNum() const
{
	return num_indices;
}

GLfloat * Primitive::GetVertices() const
{
	return vertices;
}

GLuint * Primitive::GetIndices() const
{
	return indices;
}