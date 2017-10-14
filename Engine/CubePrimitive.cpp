#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Application.h"
#include "BasicGeometry.h"
#include "CubePrimitive.h"

CubePrimitive::CubePrimitive() : Primitive(24, 36)	//num_vertices, num_indices
{}

CubePrimitive::~CubePrimitive()
{}

bool CubePrimitive::LoadCube()
{
	cube = new AABB(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	const uint divisions = 1;

	math::float3 all_vertex[36 * divisions];
	cube->Triangulate(divisions, divisions, divisions, all_vertex, NULL, NULL, true);

	App->primitives->Vertex2VertexIndices(all_vertex, 36 * divisions, vertices, indices);

	//Save vertex
	glGenBuffers(1, (GLuint*)&vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices, vertices, GL_STATIC_DRAW);

	//Save index
	glGenBuffers(1, (GLuint*)&indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);

	//------------------------- CLEAR BUFFERS -------------------------
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != 0)
	{
		LOG("Error Loading Basic Primitives");
		return false;
	}
	return true;
}
