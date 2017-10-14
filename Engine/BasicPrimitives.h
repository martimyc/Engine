#ifndef BASIC_PRIMITIVES_H
#define BASIC_PRIMITIVES_H

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
//-------------------

class BasicPrimitives : public Module
{
private:

	CubePrimitive* cube = nullptr;

public:
	BasicPrimitives(const char* name, bool start_enabled = true);
	~BasicPrimitives();

	bool Init();

	bool LoadPrimitives();
	void Vertex2VertexIndices(math::float3* all_vertices, uint num_all_vertices, GLfloat* vertices, GLuint* indices);
	bool GetPrimitiveId(PRIMITIVE_TYPE primitive, uint& vertex_id, uint& vertices_num, GLfloat* vertices, uint& indices_id, uint& indices_num, GLuint* indices);
};


#endif // BASIC_PRIMITIVES_H