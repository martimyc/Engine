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

bool BasicPrimitives::GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, float* vertices, uint& indices_id, uint& indices_num, uint* indices)
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

void BasicPrimitives::Vertex2VertexIndices(math::float3 * all_vertex, GLfloat * vertex, GLuint * indices)
{

}