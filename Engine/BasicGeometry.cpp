#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Primitive.h"
#include "CubePrimitive.h"
#include "SpherePrimitive.h"
#include "GameObject.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "Application.h"
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

bool BasicGeometry::GetPrimitiveId(PRIMITIVE_TYPE primitive, Mesh * mesh) const
{
	uint num_components = 2;
	GLuint* uvs_id = new GLuint[1];
	GLfloat** uvs = new GLfloat*[1];
	switch (primitive)
	{
	case NULL_PRIMITIVE:
		LOG("Error Getting Primitive ID: NULL_PRIMITIVE");
		return false;
	case PRIMITIVE_CUBE:
		mesh->SetVertices(cube->GetVertexId(), cube->GetVerticesNum(), cube->GetVertices());
		mesh->SetIndices(cube->GetIndicesId(), cube->GetIndicesNum(), cube->GetIndices());		
		uvs_id[0] =	cube->GetUVsId();
		uvs[0] = cube->GetUVs();
		//memcpy(uvs[0], cube->GetUVs(), sizeof(GLfloat)*cube->GetVerticesNum()*num_components);
		mesh->SetUVs(1, &num_components, uvs_id, uvs);
		break;
	case PRIMITIVE_SPHERE:
		mesh->SetVertices(sphere->GetVertexId(), sphere->GetVerticesNum(), sphere->GetVertices());
		mesh->SetIndices(sphere->GetIndicesId(), sphere->GetIndicesNum(), sphere->GetIndices());
		break;
	default:
		LOG("Error Getting Primitive ID");
		return false;
	}
	return true;
}


GameObject* BasicGeometry::Create3DCube()
{
	GameObject* go = App->scene_manager->CreateGameObject();
	MeshFilter* mesh = new MeshFilter (App->scene_manager->CreateMesh());
	go->AddComponent(mesh);
	//GetPrimitiveId(PRIMITIVE_CUBE, mesh); //This should just return a mesh
	//go->AddComponent(mesh);
	return go;
}

GameObject* BasicGeometry::CreateSphere()
{
	GameObject* go = App->scene_manager->CreateGameObject();
	MeshFilter* mesh = new MeshFilter(App->scene_manager->CreateMesh());
	go->AddComponent(mesh);
	//GetPrimitiveId(PRIMITIVE_SPHERE, mesh); //This should just return a mesh
	//go->AddComponent(mesh); 
	return go;
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