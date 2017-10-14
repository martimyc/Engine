#ifndef _MODULE_BASIC_GEOMETRY_H
#define _MODULE_BASIC_GEOMETRY_H

#include "glew\include\GL\glew.h"
#include "Module.h"

enum PRIMITIVE_TYPE
{
	NULL_PRIMITIVE = 0,
	PRIMITIVE_POINT,
	PRIMITIVE_LINE,
	PRIMITIVE_CUBE,
	PRIMITIVE_PLANE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_RAY,
	PRIMITIVE_CAPSULE,
	PRIMITIVE_FRUSTUM
};

//forward declarations
namespace math
{
	class float3;
}
class CubePrimitive;
class SpherePrimitive;
class GameObject;
//-------------------

class BasicGeometry : public Module
{
private:

	CubePrimitive*		cube = nullptr;
	SpherePrimitive*	sphere = nullptr;

public:
	BasicGeometry(const char* name, bool start_enabled = true);
	~BasicGeometry();

	bool Init();

	bool LoadPrimitives();
	void Vertex2VertexIndices(math::float3* all_vertices, uint num_all_vertices, GLfloat* vertices, GLuint* indices);
	bool GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, GLfloat* vertices, uint& indices_id, uint& indices_num, GLuint* indices);

	GameObject& Create3DCube();
	GameObject& CreateSphere();
};


#endif // _MODULE_BASIC_GEOMETRY_H