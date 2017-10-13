#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "BasicPrimitives.h"

BasicPrimitives::BasicPrimitives(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

BasicPrimitives::~BasicPrimitives()
{}


bool BasicPrimitives::Init()
{
	if (LoadPrimitives())
		return true;
	return false;
}

bool BasicPrimitives::LoadPrimitives()
{
	//------------------------------CUBE START------------------------------
	GLfloat cube_vert[24] =
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

	GLuint cube_indices[36] =
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
	uint divisions = 1;

	//Save vertex
	std::vector<math::float3> cube_vert;
	cube_vert.reserve(cube->NumVerticesInTriangulation(divisions, divisions, divisions));
	cube->Triangulate(divisions, divisions, divisions, cube_vert.data(), NULL, NULL, true);
	*/
	glGenBuffers(1, (GLuint*)&cube_vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vert), cube_vert, GL_STATIC_DRAW);

	//Save index
	//std::vector<math::float3> cube_indices;
	//cube_indices.reserve(cube_num_indices);
	//cube->//Triangulate(1, 1, 1, cube_indices.data(), NULL, NULL, true);
	glGenBuffers(1, (GLuint*)&cube_indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	//------------------------------CUBE END------------------------------


	//------------------------------CLEAR BUFFERS------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != 0)
	{
		LOG("Error Loading Basic Primitives");
		return false;
	}
	return true;
}

bool BasicPrimitives::GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, uint& indices_id, uint& indices_num)
{
	switch (primitive)
	{
	case NULL_PRIMITIVE:
		LOG("Error Getting Primitive ID: NULL_PRIMITIVE");
		return false;
	case PRIMITIVE_CUBE:
		vertex_id = cube_vertex_id;
		vertices_num = cube_num_vertices;
		indices_id = cube_indices_id;
		indices_num = cube_num_indices;
		break;
	default:
		LOG("Error Getting Primitive ID");
		return false;
	}
	return true;
}