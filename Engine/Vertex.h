#ifndef VERTEX
#define VERTEX

#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\MathGeoLibFwd.h"

namespace math
{
	class Triangle;
	class LineSegment;
}

struct Vertex
{
	math::vec vertex;

	std::vector<const math::Triangle*> triangles;

	Vertex();

	Vertex(const float* ptr);

	Vertex(float x, float y, float z);

	~Vertex();

	float operator [] (int i) const;

	bool operator == (const math::vec& vec) const;
	bool operator == (const Vertex& v) const;

	bool operator != (const math::vec& vec) const;
	bool operator != (const Vertex& v) const;

	const Vertex& operator = (const Vertex& v);
	const Vertex& operator = (const math::vec& v);

	void AddTriangle(const math::Triangle* new_triangle);

	bool CheckCollision(const LineSegment* ray, math::Triangle& triangle) const;

	const math::vec GetMaxPos() const;
	const math::vec GetMinPos() const;
};

#endif // !VERTEX




