#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\MathGeoLibFwd.h"

#define MAX_NUM_OBJECTS 4

namespace Geo
{
	struct Vertex;
}

enum PARTITION_AXIS
{
	X = 0,
	Y = 1,
	Z = 2,
	NO_PARTITION
};

class KDTNodeVertex
{
private:
	AABB* limits;
	const Geo::Vertex* vertices[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeVertex* childs[2];
	float median;

public:
	KDTNodeVertex(const math::vec min_point, const math::vec max_point);
	KDTNodeVertex(const AABB& limits);
	~KDTNodeVertex();

	bool SubDivide3D(const Geo::Vertex* new_vertex);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedian(PARTITION_AXIS partition_axis, const Geo::Vertex* new_vertex) const;

	bool AddVertex(const Geo::Vertex* new_vertex);

	bool AddToCorrectChild(const Geo::Vertex* new_vertex);

	bool RemoveVertex(const Geo::Vertex* new_vertex);

	void ReArrange();

	bool Empty() const;

	void GetVertices(std::vector<const Geo::Vertex*>& vec) const;

	bool IsIn(const Geo::Vertex* new_vertex) const;

	void Draw() const;

	bool AllSamePos(const Geo::Vertex* new_vertex) const;
};

class KDTreeVertex
{
private:
	KDTNodeVertex* root;
	AABB limits;

	bool ReCalculate(Geo::Vertex* new_vertex);
public:
	KDTreeVertex();
	~KDTreeVertex();

	bool AddVertex(Geo::Vertex* new_vertex);
	bool AddVertices(const GLfloat*const new_vertices, int num_vertices, const GLuint*const new_indices, int num_indices);

	void Draw() const;
};

#endif // !KDTREE_VERTEX