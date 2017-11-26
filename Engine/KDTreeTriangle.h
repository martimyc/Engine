#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\Geometry\Triangle.h"
#include "MathGeoLib\src\Geometry\AABB.h"

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

class KDTNodeTriangle
{
private:
	AABB limits;
	math::Triangle triangles[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeTriangle* childs[2];
	float median;

public:
	KDTNodeTriangle(const math::vec min_point, const math::vec max_point);
	KDTNodeTriangle(const AABB& limits);
	~KDTNodeTriangle();

private:
	bool SubDivide3D(const math::Triangle& new_vertex, unsigned int& num_subdivisions);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedian(PARTITION_AXIS partition_axis, const math::Triangle& new_vertex) const;

	float FindBestMedianX(const math::Triangle& new_vertex) const;

	float FindBestMedianY(const math::Triangle& new_vertex) const;

	float FindBestMedianZ(const math::Triangle& new_vertex) const;

	bool AllSamePos(const math::Triangle& new_vertex) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const math::Triangle& new_vertex, unsigned int& num_subdivisions);

public:
	bool AddTriangle(const math::Triangle& new_vertex, unsigned int& num_subdivisions);

	bool RemoveTriangle(const math::Triangle& new_vertex);

	void GetTriangles(std::vector<math::Triangle>& vec) const;

	bool IsIn(const math::Triangle& new_vertex) const;
	bool AllIn(const math::Triangle& new_vertex) const;

	void Draw() const;

	bool UpdateTriangle(const math::Triangle& updated_go);

	void DeleteHirarchy();

	bool RayCollisionKDT(const math::LineSegment* ray, math::Triangle& triangle) const;
};

class KDTreeTriangle
{
private:
	KDTNodeTriangle* root;

public:
	KDTreeTriangle();
	~KDTreeTriangle();

	bool AddTriangles(const float*const new_vertices, const unsigned int*const new_indices, int num_indices);

	void Draw() const;

	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle) const;
};

namespace KDTT
{
	//Subdivide priority queue operators
	struct CompareMaxPositionsX
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};

	struct CompareMaxPositionsY
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};

	struct CompareMaxPositionsZ
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};

	struct CompareMinPositionsX
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};

	struct CompareMinPositionsY
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};

	struct CompareMinPositionsZ
	{
		bool operator()(const math::Triangle& go1, const math::Triangle& go2);
	};
}
#endif // !KDTREE_VERTEX