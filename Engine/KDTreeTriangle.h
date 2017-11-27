#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Geometry\Triangle.h"

#define MAX_NUM_OBJECTS 7
#define MAX_SUBDIVISIONS 10

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
	math::vec max;
	math::vec min;

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeTriangle* childs[2];
	float median;

public:
	KDTNodeTriangle(const math::vec min_point, const math::vec max_point);
	KDTNodeTriangle(const AABB& limits);
	~KDTNodeTriangle();

private:
	bool SubDivide3D(const math::Triangle& new_triangle, unsigned int& num_subdivisions);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedianX(const math::Triangle& new_triangle) const;

	float FindBestMedianY(const math::Triangle& new_triangle) const;

	float FindBestMedianZ(const math::Triangle& new_triangle) const;

	bool AllSamePos(const math::Triangle& new_triangle) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const math::Triangle& new_triangle, unsigned int& num_subdivisions);

	void UpdateMax(const math::vec& vec);
	void UpdateMin(const math::vec& vec);

	void UpdateMax();
	void UpdateMin();

	const math::vec& GetMax() const;
	const math::vec& GetMin() const;

	math::AABB GetMaxMinAABB() const;

	bool IsIn(const math::Triangle& new_triangle) const;

	bool WithinLimits(const math::vec& vec) const;

public:
	bool AddTriangle(const math::Triangle& new_triangle, unsigned int& num_subdivisions);

	bool RemoveTriangle(const math::Triangle& new_triangle);

	void GetTriangles(std::vector<math::Triangle>& vec) const;

	void Draw() const;

	void DeleteHirarchy();

	bool RayCollisionKDT(const math::LineSegment* ray, float& shortest_distance) const;

	bool Inside(const math::Triangle& new_triangle) const;
};

class KDTreeTriangle
{
private:
	KDTNodeTriangle* root;

	bool ReCalculate(const math::Triangle& new_triangle);

public:
	KDTreeTriangle();
	~KDTreeTriangle();

	bool AddTriangle(const math::Triangle& new_triangle);

	bool RemoveTriangle(const math::Triangle& new_triangle);

	bool AddTriangles(const float*const new_vertices, const unsigned int*const new_indices, int num_indices, const math::vec& max, const math::vec& min);

	void Draw() const;

	float RayCollisionKDT(const LineSegment* ray) const;
};

#endif // !KDTREE_VERTEX