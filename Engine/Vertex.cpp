#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\Triangle.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "Globals.h"
#include "Vertex.h"

Vertex::Vertex() : vertex(math::float3::inf)
{}

Vertex::Vertex(const float * ptr)
{
	memcpy(&vertex, ptr, sizeof(float) * 3);
}

Vertex::Vertex(float x, float y, float z) : vertex(x, y, z)
{}

Vertex::~Vertex()
{}

float Vertex::X() const
{
	return vertex.x;
}

float Vertex::Y() const
{
	return vertex.y;
}

float Vertex::Z() const
{
	return vertex.z;
}

const math::vec & Vertex::Vec() const
{
	return vertex;
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
	if (abs(vertex.x - v.X()) > EPSILON)
		return false;
	if (abs(vertex.y - v.Y()) > EPSILON)
		return false;
	if (abs(vertex.z - v.Z()) > EPSILON)
		return false;

	if (triangles.size() == v.triangles.size())
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

	for (std::vector<const math::Triangle*>::const_iterator it = v.triangles.begin(); it != v.triangles.end(); ++it)
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

void Vertex::AddTriangles(const Vertex& vertex)
{
	for (std::vector<const math::Triangle*>::const_iterator it = vertex.triangles.begin(); it != vertex.triangles.end(); ++it)
	{
		bool already_in = false;
		for (std::vector<const math::Triangle*>::const_iterator it2 = triangles.begin(); it2 != triangles.end(); ++it2)
			if (*it == *it2)
				already_in = true;

		if(already_in == false)
			triangles.push_back(*it);
	}		
}

bool Vertex::CheckCollision(const math::LineSegment * ray, float shortest_distance) const
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

void Vertex::Infinite()
{
	vertex = math::vec::inf;
	triangles.clear();
}

bool Vertex::IsFinite() const
{
	return vertex.IsFinite();
}

void Vertex::Draw(float red, float green, float blue, float alpha) const
{
	glPointSize(25.0f);

	glColor4f(red, green, blue, alpha);

	glBegin(GL_POINTS);

	glVertex3f(vertex.x, vertex.y, vertex.z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
