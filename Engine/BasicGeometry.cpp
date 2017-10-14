#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Primitive.h"
#include "CubePrimitive.h"
#include "SpherePrimitive.h"
#include "GameObject.h"
#include "Mesh.h"
#include "BasicGeometry.h"

BasicGeometry::BasicGeometry(const char * name, bool start_enabled) : Module(name, start_enabled)
{
	cube = new CubePrimitive();
	sphere = new SpherePrimitive();
}

BasicGeometry::~BasicGeometry()
{
	DELETE_PTR(cube);
	DELETE_PTR(sphere);
}


bool BasicGeometry::Init()
{
	return LoadPrimitives();
}

bool BasicGeometry::LoadPrimitives()
{
	bool ret = true;
	ret = cube->LoadCube();
	ret = sphere->LoadSphere();
	return ret;
}

bool BasicGeometry::GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, GLfloat* vertices, uint& indices_id, uint& indices_num, GLuint* indices)
{
	switch (primitive)
	{
	case NULL_PRIMITIVE:
		LOG("Error Getting Primitive ID: NULL_PRIMITIVE");
		return false;
	case PRIMITIVE_CUBE:
		vertex_id = cube->GetVertexId();
		vertices_num = cube->GetVerticesNum();
		vertices = cube->GetVertices();
		indices_id = cube->GetIndicesId();
		indices_num = cube->GetIndicesNum();
		indices = cube->GetIndices();
		break;
	case PRIMITIVE_SPHERE:
		vertex_id = sphere->GetVertexId();
		vertices_num = sphere->GetVerticesNum();
		vertices = sphere->GetVertices();
		indices_id = sphere->GetIndicesId();
		indices_num = sphere->GetIndicesNum();
		indices = sphere->GetIndices();
		break;
	default:
		LOG("Error Getting Primitive ID");
		return false;
	}
	return true;
}

GameObject& BasicGeometry::Create3DCube()
{
	GameObject* go = new GameObject();
	uint vertex_id = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint indices_id = 0;
	uint num_indices = 0;
	uint* indices = nullptr;
	GetPrimitiveId(PRIMITIVE_CUBE, vertex_id, num_vertices, vertices, indices_id, num_indices, indices);
	go->AddComponent(new Mesh(vertex_id, num_vertices, vertices, indices_id, num_indices, indices, 0, 0));
	return *go;
}

GameObject & BasicGeometry::CreateSphere()
{
	GameObject* go = new GameObject();
	uint vertex_id = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint indices_id = 0;
	uint num_indices = 0;
	uint* indices = nullptr;
	GetPrimitiveId(PRIMITIVE_SPHERE, vertex_id, num_vertices, vertices, indices_id, num_indices, indices);
	go->AddComponent(new Mesh(vertex_id, num_vertices, vertices, indices_id, num_indices, indices, 0, 0));
	return *go;
}

void BasicGeometry::Vertex2VertexIndices(math::float3* all_vertices, uint num_all_vertices, GLfloat* vertices, GLuint* indices)
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