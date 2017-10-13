#include "glew\include\GL\glew.h"
#include "CubePrimitive.h"

CubePrimitive::CubePrimitive()
{
	//------------------------------CUBE START------------------------------

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

	/*
	AABB* cube = new AABB(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	const uint divisions = 1;

	math::float3 cube_all_vert[36 * divisions];
	cube->Triangulate(divisions, divisions, divisions, cube_all_vert, NULL, NULL, true);

	GLfloat cube_vertex[24 * divisions];
	GLuint cube_indices[36 * divisions];

	Vertex2VertexIndices(cube_all_vert, cube_vertex, cube_indices);

	//Save vertex
	glGenBuffers(1, (GLuint*)&cube_vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex), cube_vertex, GL_STATIC_DRAW);
	*/


	//Save index
	/*
	std::vector<math::float3> cube_indices;
	cube_indices.reserve(cube_num_indices);
	//cube->//Triangulate(1, 1, 1, cube_indices.data(), NULL, NULL, true);
	glGenBuffers(1, (GLuint*)&cube_indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices.data() , GL_STATIC_DRAW);
	//------------------------------CUBE END------------------------------
	*/

	//------------------------------CLEAR BUFFERS------------------------------
	/*
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != 0)
	{
	LOG("Error Loading Basic Primitives");
	return false;
	}
	*/
	//FROM HERE---------------------------------------


	//------------------------------CUBE START------------------------------

	/*
	AABB* cube = new AABB(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	uint divisions = 1;

	//Save vertex
	std::vector<math::float3> cube_vert;
	cube_vert.reserve(cube->NumVerticesInTriangulation(divisions, divisions, divisions));
	cube->Triangulate(divisions, divisions, divisions, cube_vert.data(), NULL, NULL, true);
	*/
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


	//------------------------------CLEAR BUFFERS------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != 0)
		LOG("Error Loading Basic Cube Primitive");

}