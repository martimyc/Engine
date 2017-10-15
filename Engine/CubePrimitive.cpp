#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Math\float2.h"
#include "Application.h"
#include "BasicGeometry.h"
#include "CubePrimitive.h"

CubePrimitive::CubePrimitive() : Primitive(24, 36)	//num_vertices, num_indices, num_uvs
{}

CubePrimitive::~CubePrimitive()
{}

bool CubePrimitive::LoadCube()
{
	cube = new AABB(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	const uint divisions = 1;

	math::float3 all_vertices[36 * divisions];
	math::float2 all_uvs[36 * divisions];
	cube->Triangulate(divisions, divisions, divisions, all_vertices, NULL, all_uvs, false);

	App->primitives->Vertex2VertexIndices(all_vertices, 36 * divisions, vertices, indices);

	//Save vertex
	glGenBuffers(1, (GLuint*)&vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices, vertices, GL_STATIC_DRAW);

	//Save index
	glGenBuffers(1, (GLuint*)&indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);

	//Save UVs
	memcpy(uvs, vertices, num_indices * sizeof(GLfloat) * 3);
	glGenBuffers(1, (GLuint*) &(uvs_id));
	glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

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
