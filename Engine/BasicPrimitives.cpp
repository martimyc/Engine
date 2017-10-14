#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "CubePrimitive.h"
#include "BasicPrimitives.h"

BasicPrimitives::BasicPrimitives(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

BasicPrimitives::~BasicPrimitives()
{
	DELETE_PTR(cube);
}


bool BasicPrimitives::Init()
{
	if (LoadPrimitives())
		return true;
	return false;
}

bool BasicPrimitives::LoadPrimitives()
{
	cube = new CubePrimitive();
	return true;
}

bool BasicPrimitives::GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, GLfloat* vertices, uint& indices_id, uint& indices_num, GLuint* indices)
{
	switch (primitive)
	{
	case NULL_PRIMITIVE:
		LOG("Error Getting Primitive ID: NULL_PRIMITIVE");
		return false;
	case PRIMITIVE_CUBE:
		vertex_id = cube->vertex_id;
		vertices_num = cube->num_vertices;
		vertices = cube->vertices;
		indices_id = cube->indices_id;
		indices_num = cube->num_indices;
		indices = cube->indices;
		break;
	default:
		LOG("Error Getting Primitive ID");
		return false;
	}
	return true;
}

void BasicPrimitives::Vertex2VertexIndices(math::float3* all_vertices, uint num_all_vertices, GLfloat* vertices, GLuint* indices)
{
	std::vector<math::float3> indexed_vertices;
	uint num_vertices = 0;
	uint num_indices = 0;

	for (int i = 0; i < num_all_vertices; i++)
	{
		std::vector<math::float3>::iterator it = std::find(indexed_vertices.begin(), indexed_vertices.end(), all_vertices[i]);	//cant be const for std::distance
		if (it == indexed_vertices.end())
		{
			indexed_vertices.push_back(all_vertices[i]);

			vertices[num_vertices * 3]		= all_vertices[i].x;
			vertices[num_vertices * 3 + 1]	= all_vertices[i].y;
			vertices[num_vertices * 3 + 2]	= all_vertices[i].z;

			indices[num_indices++] = num_vertices;

			num_vertices++;
		}
		else
			indices[num_indices++] = std::distance(indexed_vertices.begin(), it);
	}
}