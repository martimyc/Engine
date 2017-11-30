#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Geometry\Triangle.h"

#define MAX_NUM_OBJECTS 13
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
	math::Triangle triangles[MAX_NUM_OBJECTS];
	AABB limits;
	math::vec max_vec;
	math::vec min_vec;

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeTriangle* childs[2];
	float median;

	KDTNodeTriangle();

public:
	KDTNodeTriangle(const math::vec min_point, const math::vec max_point);
	KDTNodeTriangle(const AABB& limits);
	~KDTNodeTriangle();

private:
	bool SubDivide3D(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedianX(const math::Triangle& new_triangle) const;

	float FindBestMedianY(const math::Triangle& new_triangle) const;

	float FindBestMedianZ(const math::Triangle& new_triangle) const;

	bool AllSamePos(const math::Triangle& new_triangle) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes);

	void UpdateMax(math::vec vec);
	void UpdateMin(math::vec vec);

	void UpdateMax();
	void UpdateMin();

	const math::vec& GetMax() const;
	const math::vec& GetMin() const;

	math::AABB GetMaxMinAABB() const;

	bool IsIn(const math::Triangle& new_triangle) const;

	bool WithinLimits(const math::vec& vec) const;

	bool Intersects(const math::LineSegment* ray) const;

	float GetDistance() const;

public:
	bool AddTriangle(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes);

	bool RemoveTriangle(const math::Triangle& new_triangle, unsigned int& num_nodes, unsigned int& bytes);

	void GetTriangles(std::vector<math::Triangle>& vec) const;

	void Draw() const;

	void DeleteHirarchy();

	bool RayCollisionKDT(const math::LineSegment* ray, float& shortest_distance, unsigned int& num_checks) const;

	bool Inside(const math::Triangle& new_triangle) const;
	
	void Save(char ** iterator) const;
	void Load(char ** iterator);

	void GetSaveSize(unsigned int& save_size) const;
};

class KDTreeTriangle
{
private:
	KDTNodeTriangle* root;

	unsigned int bytes;
	
	unsigned int num_nodes;

	bool draw;

	bool ReCalculate(const math::Triangle& new_triangle);

public:
	KDTreeTriangle();
	~KDTreeTriangle();

	bool AddTriangle(const math::Triangle& new_triangle);

	bool RemoveTriangle(const math::Triangle& new_triangle);

	bool AddTriangles(const float*const new_vertices, const unsigned int*const new_indices, int num_indices, const math::vec& max, const math::vec& min);

	void Draw() const;

	float RayCollisionKDT(const LineSegment* ray, unsigned int& num_checks) const;

	void Inspector();

	void Save(char ** iterator) const;
	void Load(char ** iterator);

	unsigned int GetSaveSize() const;
};

#endif // !KDTREE_VERTEX