#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Vertex.h"

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

class KDTNodeVertex
{
private:
	AABB limits;
	Vertex vertices[MAX_NUM_OBJECTS];
	math::vec max;
	math::vec min;

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

	float FindBestMedianX(const Vertex& new_vertex) const;

	float FindBestMedianY(const Vertex& new_vertex) const;

	float FindBestMedianZ(const Vertex& new_vertex) const;

	bool AllSamePos(const Vertex& new_vertex) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const Vertex& new_vertex, unsigned int& num_subdivisions);

	void UpdateMax(const math::vec& vec);
	void UpdateMin(const math::vec& vec);

	void UpdateMax();
	void UpdateMin();

	const math::vec& GetMax() const;
	const math::vec& GetMin() const;

	math::AABB GetMaxMinAABB() const;

	bool IsIn(const Vertex& new_vertex) const;

public:
	bool AddVertex(const Vertex& new_vertex, unsigned int& num_subdivisions);

	bool RemoveVertex(const Vertex& new_vertex);

	void GetVertices(std::vector<Vertex>& vec) const;

	void Draw() const;

	void DeleteHirarchy();

	bool RayCollisionKDT(const math::LineSegment* ray, float& shortest_distance) const;

	bool Inside(const Vertex& new_vertex) const;
};

class KDTreeVertex
{
private:
	KDTNodeVertex* root;

	std::vector<math::Triangle> triangles;

	bool ReCalculate(const Vertex& new_vertex);

public:
	KDTreeVertex();
	~KDTreeVertex();

	bool AddVertex(const Vertex& new_vertex);

	bool RemoveVertex(const Vertex& new_vertex);

	bool AddVertices(const float*const new_vertices, int num_vertices, const unsigned int*const new_indices, int num_indices);

	void Draw() const;

	float RayCollisionKDT(const LineSegment* ray) const;
};

#endif // !KDTREE_VERTEX