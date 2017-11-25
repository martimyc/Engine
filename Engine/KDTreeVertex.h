#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\MathGeoLibFwd.h"
#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Vertex.h"

#define MAX_NUM_OBJECTS 3
#define MAX_SUBDIVISIONS 5

namespace math
{
	class Triangle;
	class LineSegment;
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
	AABB limits;
	Vertex vertices[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeVertex* childs[2];
	float median;

public:
	KDTNodeVertex(const math::vec min_point, const math::vec max_point);
	KDTNodeVertex(const AABB& limits);
	~KDTNodeVertex();

private:
	bool SubDivide3D(const Vertex& new_vertex, unsigned int& num_subdivisions);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedian(PARTITION_AXIS partition_axis, const Vertex& new_vertex) const;

	float FindBestMedianX(const Vertex& new_vertex) const;

	float FindBestMedianY(const Vertex& new_vertex) const;

	float FindBestMedianZ(const Vertex& new_vertex) const;

	bool AllSamePos(const Vertex& new_vertex) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const Vertex& new_vertex, unsigned int& num_subdivisions);

public:
	bool AddVertex(const Vertex& new_vertex, unsigned int& num_subdivisions);

	bool RemoveVertex(const Vertex& new_vertex);

	void GetVertices(std::vector<Vertex>& vec) const;

	bool IsIn(const Vertex& new_vertex) const;
	bool AllIn(const Vertex& new_vertex) const;

	void Draw() const;

	bool UpdateGO(const Vertex& updated_go);

	void DeleteHirarchy();

	bool RayCollisionKDT(const math::LineSegment* ray, math::Triangle& triangle) const;
};

class KDTreeVertex
{
private:
	KDTNodeVertex* root;

public:
	KDTreeVertex();
	~KDTreeVertex();

	bool AddVertices(const float*const new_vertices, int num_vertices, const unsigned int*const new_indices, int num_indices);

	void Draw() const;

	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle) const;
};

namespace KDTV
{
	//Subdivide priority queue operators
	struct CompareMaxPositionsX
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};

	struct CompareMaxPositionsY
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};

	struct CompareMaxPositionsZ
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};

	struct CompareMinPositionsX
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};

	struct CompareMinPositionsY
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};

	struct CompareMinPositionsZ
	{
		bool operator()(const Vertex& go1, const Vertex& go2);
	};
}
#endif // !KDTREE_VERTEX