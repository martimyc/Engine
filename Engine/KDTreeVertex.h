#ifndef KDTREE_VERTEX
#define KDTREE_VERTEX

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"

#define MAX_NUM_OBJECTS 3
#define MAX_SUBDIVISIONS 5

namespace Geo
{
	class Vertex;
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
	const Geo::Vertex* vertices[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeVertex* childs[2];
	float median;

public:
	KDTNodeVertex(const math::vec min_point, const math::vec max_point);
	KDTNodeVertex(const AABB& limits);
	~KDTNodeVertex();

private:
	bool SubDivide3D(const Geo::Vertex* new_vertex, unsigned int& num_subdivisions);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedian(PARTITION_AXIS partition_axis, const Geo::Vertex* new_vertex) const;

	float FindBestMedianX(const Geo::Vertex* new_vertex) const;

	float FindBestMedianY(const Geo::Vertex* new_vertex) const;

	float FindBestMedianZ(const Geo::Vertex* new_vertex) const;

	bool AllSamePos(const Geo::Vertex* new_vertex) const;

	void ReArrange();

	bool Empty() const;

	bool AddToCorrectChild(const Geo::Vertex* new_vertex, unsigned int& num_subdivisions);

public:
	bool AddVertex(const Geo::Vertex* new_vertex, unsigned int& num_subdivisions);

	bool RemoveVertex(const Geo::Vertex* new_vertex);

	void GetGameObjects(std::vector<const Geo::Vertex*>& vec) const;

	bool IsIn(const Geo::Vertex* new_vertex) const;
	bool AllIn(const Geo::Vertex* new_vertex) const;

	void Draw() const;

	bool UpdateGO(const Geo::Vertex* updated_go);

	void DeleteHirarchy();

	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle) const;
};

class KDTreeVertex
{
private:
	KDTNodeVertex* root;

	bool ReCalculate(const Geo::Vertex* new_vertex);
public:
	KDTreeVertex();
	~KDTreeVertex();

	bool AddVertex(const Geo::Vertex* new_vertex);
	bool AddVertices(const float*const new_vertices, int num_vertices, const unsigned int*const new_indices, int num_indices);

	bool RemoveVertex(const Geo::Vertex* new_vertex);

	bool UpdateGO(const Geo::Vertex* updated_go);

	void Draw() const;

	bool RayCollisionKDT(const LineSegment* ray, Triangle& triangle) const;
};

namespace KDTV
{
	//Subdivide priority queue operators
	struct CompareMaxPositionsX
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};

	struct CompareMaxPositionsY
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};

	struct CompareMaxPositionsZ
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};

	struct CompareMinPositionsX
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};

	struct CompareMinPositionsY
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};

	struct CompareMinPositionsZ
	{
		bool operator()(const Geo::Vertex * go1, const Geo::Vertex * go2);
	};
}
#endif // !KDTREE_VERTEX