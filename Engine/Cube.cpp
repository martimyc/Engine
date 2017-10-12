#include "Cube.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"


// Constructors =================================
Cube::Cube() :Primitive(PRIMITIVE_CUBE)
{
	cube = new math::AABB();
}

Cube::Cube(const Cube & copy) : Primitive(copy)
{
	cube = new math::AABB(*copy.cube);
}

// Destructors ==================================
Cube::~Cube()
{
	DELETE_PTR(cube);
}

// Game Loop ====================================
void Cube::Initialize()
{
	num_vertices = 36;

	//Get cube triangulation
	std::vector<math::float3> all_vertex;
	all_vertex.reserve(num_vertices);
	cube->Triangulate(1, 1, 1, all_vertex.data(), NULL, NULL, true);
}