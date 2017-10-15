#include "MathGeoLib\src\Geometry\Sphere.h"
#include "Application.h"
#include "BasicGeometry.h"
#include "SpherePrimitive.h"

SpherePrimitive::SpherePrimitive() : Primitive(SPHERE_NUM_VERTICES, SPHERE_NUM_VERTICES)	//num_vertices, num_indices
{}

SpherePrimitive::~SpherePrimitive()
{}

bool SpherePrimitive::LoadSphere()
{
	sphere = new Sphere(vec(0.0f, 0.0f, 0.0f), 1.0f);

	math::float3 all_vertices[SPHERE_NUM_VERTICES];
	sphere->Triangulate(all_vertices, NULL, NULL, SPHERE_NUM_VERTICES, false);

	App->primitives->Vertex2VertexIndices(all_vertices, SPHERE_NUM_VERTICES, vertices, indices);

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
