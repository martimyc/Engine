#include "MathGeoLib\src\Geometry\Triangle.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "Vertex.h"

Vertex::Vertex() : vertex(math::float3::inf)
{
	if (this->vertex == math::float3::inf)
		float a = 0.0f;
}

Vertex::Vertex(const float * ptr) : vertex(ptr[0], ptr[1], ptr[2])
{}

Vertex::Vertex(float x, float y, float z) : vertex(x, y, z)
{}

Vertex::~Vertex()
{
	for (std::vector<const Triangle*>::iterator it = triangles.begin(); it != triangles.end(); ++it)
		if (*it != nullptr)
			delete *it;
	triangles.clear();
}

float Vertex::operator[](int i) const
{
	if (i == 0)
		return vertex.x;
	if (i == 1)
		return vertex.y;
	if (i == 2)
		return vertex.z;
}

bool Vertex::operator==(const math::vec & vec) const
{
	return (math::float3)vertex == vec;
}

bool Vertex::operator==(const Vertex & v) const
{
	if (((math::float3)vertex == v.vertex) == false)
		return false;

	if (triangles.size() == v.triangles.size())
		return false;

	for (int i = 0; i < triangles.size(); i++)
		if (triangles[i] != v.triangles[i])
			return false;

	return true;
}

bool Vertex::operator!=(const math::vec & vec) const
{
	return ((math::float3)vertex == vec) == false;
}

bool Vertex::operator!=(const Vertex & v) const
{
	if (((math::float3)vertex == v.vertex) == false)
		return true;

	if (triangles.size() == v.triangles.size())
		return true;

	for (int i = 0; i < triangles.size(); i++)
		if (triangles[i] != v.triangles[i])
			return true;

	return false;
}

const Vertex & Vertex::operator=(const Vertex & v)
{
	vertex = v.vertex;

	for (std::vector<const math::Triangle*>::const_iterator it = v.triangles.begin(); it != triangles.end(); ++it)
		triangles.push_back(*it);

	return *this;
}

const Vertex & Vertex::operator=(const math::vec & v)
{
	vertex = v;
	triangles.clear();

	return *this;
}

void Vertex::AddTriangle(const math::Triangle * new_triangle)
{
	triangles.push_back(new_triangle);
}

bool Vertex::CheckCollision(const math::LineSegment * ray, math::Triangle & triangle) const
{
	float shortest_dist = ray->Length();
	float distance = 0.0f;

	bool hit = false;

	//Check all mesh triangles
	for (int i = 0; i < triangles.size(); i++)
	{
		if (ray->Intersects(*triangles[i], &distance, nullptr))
		{
			if (shortest_dist > distance)
			{
				hit = true;
				shortest_dist = distance;
				triangle = *triangles[i];
			}
		}
	}

	if (hit)
		return true;

	return false;
}

const math::vec Vertex::GetMaxPos() const
{
	math::vec ret(0.0f, 0.0f, 0.0f);

	for (std::vector<const math::Triangle*>::const_iterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		//X
		if ((*it)->a.x > ret.x)
			ret.x = (*it)->a.x;
		if ((*it)->b.x > ret.x)
			ret.x = (*it)->b.x;
		if ((*it)->c.x > ret.x)
			ret.x = (*it)->c.x;

		//Y
		if ((*it)->a.y > ret.y)
			ret.y = (*it)->a.y;
		if ((*it)->b.y > ret.y)
			ret.y = (*it)->b.y;
		if ((*it)->c.y > ret.y)
			ret.y = (*it)->c.y;

		//Z
		if ((*it)->a.z > ret.z)
			ret.z = (*it)->a.z;
		if ((*it)->b.z > ret.z)
			ret.z = (*it)->b.z;
		if ((*it)->c.z > ret.z)
			ret.z = (*it)->c.z;
	}

	return ret;
}

const math::vec Vertex::GetMinPos() const
{
	math::vec ret(0.0f, 0.0f, 0.0f);

	for (std::vector<const math::Triangle*>::const_iterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		//X
		if ((*it)->a.x < ret.x)
			ret.x = (*it)->a.x;
		if ((*it)->b.x < ret.x)
			ret.x = (*it)->b.x;
		if ((*it)->c.x < ret.x)
			ret.x = (*it)->c.x;

		//Y
		if ((*it)->a.y < ret.y)
			ret.y = (*it)->a.y;
		if ((*it)->b.y < ret.y)
			ret.y = (*it)->b.y;
		if ((*it)->c.y < ret.y)
			ret.y = (*it)->c.y;

		//Z
		if ((*it)->a.z < ret.z)
			ret.z = (*it)->a.z;
		if ((*it)->b.z < ret.z)
			ret.z = (*it)->b.z;
		if ((*it)->c.z < ret.z)
			ret.z = (*it)->c.z;
	}

	return ret;
}