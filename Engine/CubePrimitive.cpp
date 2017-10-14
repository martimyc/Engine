#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Application.h"
#include "BasicPrimitives.h"
#include "CubePrimitive.h"

CubePrimitive::CubePrimitive()
{	
	AABB* cube = new AABB(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	const uint divisions = 1;

	math::float3 all_vertex[36 * divisions];
	cube->Triangulate(divisions, divisions, divisions, all_vertex, NULL, NULL, true);
	
	App->primitives->Vertex2VertexIndices(all_vertex, 36 * divisions, vertices, indices);
	
	GLfloat temp_vertices[24];
	for (int i = 0; i < 24; i++)
		temp_vertices[i] = vertices[i];

	GLuint temp_indices[36];
	for (int i = 0; i < 36; i++)
		temp_indices[i] = indices[i];

	//Save vertex
	glGenBuffers(1, (GLuint*)&vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(temp_vertices), temp_vertices, GL_STATIC_DRAW);
	
	//Save index
	glGenBuffers(1, (GLuint*)&indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(temp_indices), temp_indices, GL_STATIC_DRAW);


	//------------------------------CLEAR BUFFERS------------------------------
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != 0)
		LOG("Error Loading Basic Primitives");

	

	//------------------------------CUBE START------------------------------
	/*
	glGenBuffers(1, &vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Save index
	//std::vector<math::float3> cube_indices;
	//cube_indices.reserve(cube_num_indices);
	//cube->//Triangulate(1, 1, 1, cube_indices.data(), NULL, NULL, true);
	glGenBuffers(1, &indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//------------------------------CUBE END------------------------------
	*/

}

CubePrimitive::~CubePrimitive()
{
	DELETE_ARRAY(vertices);
	DELETE_ARRAY(indices);
}