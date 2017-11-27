#ifndef VERTEX
#define VERTEX

#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\MathGeoLibFwd.h"

namespace math
{
	class Triangle;
	class LineSegment;
}

class Vertex
{
private:
	math::vec vertex;

	std::vector<const math::Triangle*> triangles;

public:
	Vertex();

	Vertex(const float* ptr);

	Vertex(float x, float y, float z);

	~Vertex();

	float X() const;
	float Y() const;
	float Z() const;
	const math::vec& Vec() const;

	float operator [] (int i) const;

	bool operator == (const math::vec& vec) const;
	bool operator == (const Vertex& v) const;

	bool operator != (const math::vec& vec) const;
	bool operator != (const Vertex& v) const;

	const Vertex& operator = (const Vertex& v);
	const Vertex& operator = (const math::vec& v);

	void AddTriangle(const math::Triangle* new_triangle);
	void AddTriangles(const Vertex& vertex);

	bool CheckCollision(const LineSegment* ray, float shortest_distance) const;

	const math::vec GetMaxPos() const;
	const math::vec GetMinPos() const;

	void Infinite();

	bool IsFinite() const;

	void Draw(float red, float green, float blue, float alpha) const;
};

#endif // !VERTEX




