#include <functional>
#include <queue>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "MathGeoLib\src\Geometry\Triangle.h"
#include "Globals.h"
#include "KDTreeVertex.h"

KDTNodeVertex::KDTNodeVertex(const math::vec min_point, const math::vec max_point) : partition_axis(NO_PARTITION), limits(min_point, max_point)
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		vertices[i].Infinite();

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeVertex::KDTNodeVertex(const AABB& limits) : partition_axis(NO_PARTITION), limits(limits)
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		vertices[i].Infinite();

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeVertex::~KDTNodeVertex() //Dont think deleting gameobjects through tree is best
{
	if (childs[0] != nullptr)
	{
		delete childs[0];
		delete childs[1];
	}
}

bool KDTNodeVertex::SubDivide3D(const Vertex& new_vertex, unsigned int& num_subdivisions)
{
	if (++num_subdivisions >= MAX_SUBDIVISIONS)
	{
		LOG("Subdividing over limit");
		return false;
	}

	if (!AllSamePos(new_vertex))
	{
		float median_x = FindBestMedianX(new_vertex);
		float median_y = FindBestMedianY(new_vertex);
		float median_z = FindBestMedianZ(new_vertex);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (AddToCorrectChild(vertices[i], num_subdivisions) == false)
				return false;

			vertices[i].Infinite();
		}

		if (AddToCorrectChild(new_vertex, num_subdivisions) == false)
			return false;

		return true;
	}
	else
		LOG("Trying to subdivide with all objects in the same place");
	return false;
}

void KDTNodeVertex::SubDivideChilds(PARTITION_AXIS partition_axis, float median)
{
	if (childs[0] != nullptr)
	{
		childs[0]->SubDivide(partition_axis, median);
		childs[1]->SubDivide(partition_axis, median);
	}
	else
		LOG("No childs to subdivide");
}

void KDTNodeVertex::SubDivide(PARTITION_AXIS partition_axis, float median)
{
	if (childs[0] == nullptr)
	{
		this->median = median;
		this->partition_axis = partition_axis;

		math::vec min_point(limits.minPoint);
		math::vec max_point;

		switch (partition_axis)
		{
		case X:
			max_point.x = median;
			max_point.y = limits.maxPoint.y;
			max_point.z = limits.maxPoint.z;
			break;
		case Y:
			max_point.x = limits.maxPoint.x;
			max_point.y = median;
			max_point.z = limits.maxPoint.z;
			break;
		case Z:
			max_point.x = limits.maxPoint.x;
			max_point.y = limits.maxPoint.y;
			max_point.z = median;
			break;
		default:
			LOG("Non standard partition axis to subdivide");
			break;
		}

		childs[0] = new KDTNodeVertex(min_point, max_point);

		max_point = limits.maxPoint;

		switch (partition_axis)
		{
		case X:
			min_point.x = median;
			min_point.y = limits.minPoint.y;
			min_point.z = limits.minPoint.z;
			break;
		case Y:
			min_point.x = limits.minPoint.x;
			min_point.y = median;
			min_point.z = limits.minPoint.z;
			break;
		case Z:
			min_point.x = limits.minPoint.x;
			min_point.y = limits.minPoint.y;
			min_point.z = median;
			break;
		default:
			LOG("Non standard partition axis to subdivide");
			break;
		}

		childs[1] = new KDTNodeVertex(min_point, max_point);
	}
	else
		LOG("Trying to subdivide node with existing childs");
}

float KDTNodeVertex::FindBestMedianX(const Vertex& new_vertex) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_vertex.X());

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (vertices[i].X() == new_vertex.X())
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (vertices[i].X() == vertices[j].X())
				{
					repeated == true;
					break;
				}
		}

		if (repeated == false)
			queue.push(vertices[i].X());
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
				queue.pop();
		}
		float first = queue.top();
		queue.pop();
		float second = queue.top();
		return (first + second) / 2.0f;
	}
	else
		return limits.CenterPoint().x;
}

float KDTNodeVertex::FindBestMedianY(const Vertex& new_vertex) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_vertex.Y());

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (vertices[i].Y() == new_vertex.Y())
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (vertices[i].Y() == vertices[j].Y())
				{
					repeated == true;
					break;
				}
		}

		if (repeated == false)
			queue.push(vertices[i].Y());
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
				queue.pop();
		}
		float first = queue.top();
		queue.pop();
		float second = queue.top();
		return (first + second) / 2.0f;
	}
	else
		return limits.CenterPoint().y;
}

float KDTNodeVertex::FindBestMedianZ(const Vertex& new_vertex) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_vertex.Z());

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (vertices[i].Z() == new_vertex.Z())
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (vertices[i].Z() == vertices[j].Z())
				{
					repeated == true;
					break;
				}
		}

		if (repeated == false)
			queue.push(vertices[i].Z());
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
				queue.pop();
		}
		float first = queue.top();
		queue.pop();
		float second = queue.top();
		return (first + second) / 2.0f;
	}
	else
		return limits.CenterPoint().z;
}

bool KDTNodeVertex::AddVertex(const Vertex& new_vertex, unsigned int& num_subdivisions)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (IsIn(new_vertex))
		{
			LOG("Adding repeated vertex");
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
				if (vertices[i].IsFinite() == false)
					if (vertices[i] == new_vertex)
					{
						vertices[i].AddTriangles(new_vertex);
						ret = true;
					}			
		}
		else
		{
			if (vertices[MAX_NUM_OBJECTS - 1].IsFinite() == false)
			{
				for (int i = 0; i < MAX_NUM_OBJECTS; i++)
					if (vertices[i].IsFinite() == false)
					{
						vertices[i] = new_vertex;
						UpdateMax(new_vertex.GetMaxPos());
						UpdateMin(new_vertex.GetMinPos());
						ret = true;
						break;
					}
			}
			else
			{
				if (SubDivide3D(new_vertex, num_subdivisions) == false)
					return false;
				else
					ret = true;
			}
		}
	}
	else
	{
		if (AddToCorrectChild(new_vertex, num_subdivisions) == false)
			return false;
		else
			ret = true;
	}

	return ret;
}

bool KDTNodeVertex::AddToCorrectChild(const Vertex& new_vertex, unsigned int& num_subdivisions)
{
	bool ret = false;

	float pos = 0.0f;

	if (new_vertex.Vec()[partition_axis] <= median)
		if (childs[0]->AddVertex(new_vertex, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (new_vertex.Vec()[partition_axis] > median)
		if (childs[1]->AddVertex(new_vertex, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (ret == false)
		LOG("Vertex is not in any child");

	return ret;
}

void KDTNodeVertex::UpdateMax(const math::vec & vec)
{
	if (vec.x > max.x)
		max.x = vec.x;
	if (vec.y > max.y)
		max.y = vec.y;
	if (vec.z > max.z)
		max.z = vec.z;
}

void KDTNodeVertex::UpdateMin(const math::vec & vec)
{
	if (vec.x < min.x)
		min.x = vec.x;
	if (vec.y < min.y)
		min.y = vec.y;
	if (vec.z < min.z)
		min.z = vec.z;
}

void KDTNodeVertex::UpdateMax()
{
	if (partition_axis == NO_PARTITION)
	{
		max = vertices[0].GetMaxPos();
		for (int i = 1; i < MAX_NUM_OBJECTS; i++)
			if (vertices[i].IsFinite())
				UpdateMax(vertices[i].GetMaxPos());
			else
				break;
	}
	else
	{
		UpdateMax(childs[0]->GetMax());
		UpdateMax(childs[1]->GetMax());
	}
}

void KDTNodeVertex::UpdateMin()
{
	if (partition_axis == NO_PARTITION)
	{
		max = vertices[0].GetMinPos();
		for (int i = 1; i < MAX_NUM_OBJECTS; i++)
			if (vertices[i].IsFinite())
				UpdateMin(vertices[i].GetMinPos());
			else
				break;
	}
	else
	{
		UpdateMin(childs[0]->GetMin());
		UpdateMin(childs[1]->GetMin());
	}
}

const math::vec& KDTNodeVertex::GetMax() const
{
	return max;
}

const math::vec& KDTNodeVertex::GetMin() const
{
	return min;
}

math::AABB KDTNodeVertex::GetMaxMinAABB() const
{
	return math::AABB(min, max);
}

bool KDTNodeVertex::RemoveVertex(const Vertex& new_vertex)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (vertices[i] == new_vertex)
			{
				math::vec v_max(vertices[i].GetMaxPos());
				math::vec v_min(vertices[i].GetMinPos());

				vertices[i].Infinite();
				ret = true;
				ReArrange();

				if (max.x == v_max.x || max.y == v_max.y || max.z == v_max.z)
					UpdateMax();
				if (min.x == v_min.x || min.y == v_min.y || min.z == v_min.z)
					UpdateMin();

				break;
			}
	}
	else
		if (childs[0]->RemoveVertex(new_vertex) || childs[1]->RemoveVertex(new_vertex))
		{
			ret = true;
			UpdateMax();
			UpdateMin();
		}

	if (partition_axis == Z && Empty())
		if (childs != nullptr)
		{
			childs[0]->DeleteHirarchy();
			childs[1]->DeleteHirarchy();
		}

	return ret;
}

void KDTNodeVertex::ReArrange()
{
	bool end = false;

	while (!end)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		{
			if (vertices[i].IsFinite() == false && vertices[i + 1].IsFinite() == true)
			{
				vertices[i] = vertices[i + 1];
				break;
			}

			if (i == MAX_NUM_OBJECTS - 1)
				end = true;
		}
	}
}

bool KDTNodeVertex::Empty() const
{
	if (partition_axis == NO_PARTITION)
	{
		if (childs[0] == nullptr)
			return true;
		else
			return false;
	}
	else
	{
		if (childs[0]->Empty() && childs[1]->Empty())
			return true;
		else
			return false;
	}
}

void KDTNodeVertex::GetVertices(std::vector<Vertex>& vec) const
{
	if (partition_axis != NO_PARTITION)
	{
		childs[0]->GetVertices(vec);
		childs[1]->GetVertices(vec);
	}
	else
	{
		if (vertices[0].IsFinite() == false)
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (vertices[i].IsFinite() == true)
				{
					bool repeated = false;
					for (std::vector<Vertex>::const_iterator it = vec.begin(); it != vec.end(); ++it)
						if ((*it) == vertices[i])
							repeated = true;
					if (!repeated)
						vec.push_back(vertices[i]);
				}
				else
					break;
			}
		}
	}
}

bool KDTNodeVertex::IsIn(const Vertex& new_vertex) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (vertices[i].IsFinite())
		{
			if (vertices[i] == new_vertex)
				return true;
		}
		else
			break;
	return false;
}

void KDTNodeVertex::Draw() const
{
	limits.Draw(1.0f, 0.0f, 1.0f, 1.0f);

	if (partition_axis != NO_PARTITION)
	{
		if (partition_axis == X)
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		else if (partition_axis == Y)
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		else if (partition_axis == Z)
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glDisable(GL_CULL_FACE);

		glBegin(GL_QUADS);

		switch (partition_axis)
		{
		case X:
			glVertex3f(median, limits.minPoint.y, limits.minPoint.z);
			glVertex3f(median, limits.minPoint.y, limits.maxPoint.z);
			glVertex3f(median, limits.maxPoint.y, limits.maxPoint.z);
			glVertex3f(median, limits.maxPoint.y, limits.minPoint.z);
			break;
		case Y:
			glVertex3f(limits.minPoint.x, median, limits.minPoint.z);
			glVertex3f(limits.minPoint.x, median, limits.maxPoint.z);
			glVertex3f(limits.maxPoint.x, median, limits.maxPoint.z);
			glVertex3f(limits.maxPoint.x, median, limits.minPoint.z);
			break;
		case Z:
			glVertex3f(limits.minPoint.x, limits.minPoint.y, median);
			glVertex3f(limits.minPoint.x, limits.maxPoint.y, median);
			glVertex3f(limits.maxPoint.x, limits.maxPoint.y, median);
			glVertex3f(limits.maxPoint.x, limits.minPoint.y, median);
			break;
		default:
			break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

		glPointSize(25.0f);

		glBegin(GL_POINTS);

		switch (partition_axis)
		{
		case X:
			glVertex3f(median, limits.minPoint.y, limits.minPoint.z);
			glVertex3f(median, limits.maxPoint.y, limits.maxPoint.z);

			glVertex3f(median, limits.minPoint.y, limits.maxPoint.z);
			glVertex3f(median, limits.maxPoint.y, limits.minPoint.z);
			break;
		case Y:
			glVertex3f(limits.minPoint.x, median, limits.minPoint.z);
			glVertex3f(limits.maxPoint.x, median, limits.maxPoint.z);

			glVertex3f(limits.minPoint.x, median, limits.maxPoint.z);
			glVertex3f(limits.maxPoint.x, median, limits.minPoint.z);
			break;
		case Z:
			glVertex3f(limits.maxPoint.x, limits.maxPoint.y, median);
			glVertex3f(limits.maxPoint.x, limits.minPoint.y, median);

			glVertex3f(limits.minPoint.x, limits.minPoint.y, median);
			glVertex3f(limits.minPoint.x, limits.maxPoint.y, median);
			break;
		default:
			break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnable(GL_CULL_FACE);

		childs[0]->Draw();
		childs[1]->Draw();
	}
	else
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (vertices[i].IsFinite() == true)
				vertices[i].Draw(0.0f, 1.0f, 1.0f, 1.0f);
			else
				break;
		}
	}
}

bool KDTNodeVertex::AllSamePos(const Vertex& new_vertex) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if ((new_vertex == vertices[i]) == false)
			return false;
	return true;
}

void KDTNodeVertex::DeleteHirarchy()
{
	median = 0;
	partition_axis = NO_PARTITION;
	DELETE_PTR(childs[0]);
	DELETE_PTR(childs[1]);
}


bool KDTNodeVertex::RayCollisionKDT(const LineSegment * ray, float& shortest_distance) const
{
	bool ret = false;
	if (partition_axis == NO_PARTITION)
	{
		if (vertices[0].IsFinite())
			return false;
		else
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (vertices[i].IsFinite() == true)
				{
					if (vertices[i].CheckCollision(ray, shortest_distance))
						ret = true;
				}
				else
					return false;
			}
		}
	}
	else
	{
		if (childs[0] != nullptr)
		{
			if(ray->Intersects(childs[0]->GetMaxMinAABB()))
				if (childs[0]->RayCollisionKDT(ray, shortest_distance))
					ret = true;

			if (ray->Intersects(childs[1]->GetMaxMinAABB()))
				if (childs[1]->RayCollisionKDT(ray, shortest_distance))
					ret = true;
		}
	}

	return ret;
}

bool KDTNodeVertex::Inside(const Vertex & new_vertex) const
{
	if (new_vertex.X() < limits.MinX())
		return false;
	if (new_vertex.X() > limits.MaxX())
		return false;

	if (new_vertex.Y() < limits.MinY())
		return false;
	if (new_vertex.Y() > limits.MaxY())
		return false;

	if (new_vertex.Z() < limits.MinZ())
		return false;
	if (new_vertex.Z() > limits.MaxZ())
		return false;

	return true;
}

bool KDTreeVertex::ReCalculate(const Vertex & new_vertex)
{
	std::vector<Vertex> all_triangles;

	root->GetVertices(all_triangles);

	delete root;

	AABB limits;
	limits.SetNegativeInfinity();

	limits.Enclose(new_vertex.GetMaxPos());
	limits.Enclose(new_vertex.GetMinPos());

	if (all_triangles.size() > 0)
	{
		for (std::vector<Vertex>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
		{
			limits.Enclose(it->GetMaxPos());
			limits.Enclose(it->GetMinPos());
		}
	}

	root = new KDTNodeVertex(limits);

	unsigned int num_subdivisions = 0;
	if (root->AddVertex(new_vertex, num_subdivisions) == false)
	{
		LOG("New vertex: '%f, %f, %f' could not be added to KDT", new_vertex.X(), new_vertex.Y(), new_vertex.Z());
		return false;
	}

	if (all_triangles.size() > 0)
	{
		for (std::vector<Vertex>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
		{
			num_subdivisions = 0;
			if (root->AddVertex(*it, num_subdivisions) == false)
			{
				LOG("Vertex: '%f, %f, %f' could not be re-added to KDT", (*it).X(), (*it).Y(), (*it).Z());
				return false;
			}
		}
	}

	return true;
}

KDTreeVertex::KDTreeVertex()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeVertex(limits);
}

KDTreeVertex::~KDTreeVertex()
{
	delete root;
	triangles.clear();
}

bool KDTreeVertex::AddVertex(const Vertex& new_vertex)
{
	if (root->Inside(new_vertex))
	{
		unsigned int num_subdivisions = 0;
		if (root->AddVertex(new_vertex, num_subdivisions) == false)
			return false;
	}
	else
		if (ReCalculate(new_vertex) == false)
			return false;

	return true;
}

bool KDTreeVertex::RemoveVertex(const Vertex& new_vertex)
{
	return root->RemoveVertex(new_vertex);
}

bool KDTreeVertex::AddVertices(const float*const new_vertices, int num_vertices, const unsigned int*const new_indices, int num_indices)
{
	std::vector<Vertex> all_vertices;
	all_vertices.reserve(num_vertices);

	AABB limits;
	limits.SetNegativeInfinity();

	for (int i = 0; i < num_vertices; i++)
	{
		all_vertices.push_back(&new_vertices[i * 3]);
		limits.Enclose(all_vertices.back().Vec());
	}

	int num_triangles = num_indices / 3;
	triangles.reserve(num_triangles);

	for (int i = 0; i < num_triangles; i++)
	{
		unsigned int v_a = new_indices[i * 3];
		unsigned int v_b = new_indices[i * 3 + 1];
		unsigned int v_c = new_indices[i * 3 + 2];

		triangles.push_back(math::Triangle());
		Triangle* tri = &triangles.back();

		memcpy(&tri->a, &new_vertices[v_a], sizeof(float) * 3);
		memcpy(&tri->b, &new_vertices[v_b], sizeof(float) * 3);
		memcpy(&tri->c, &new_vertices[v_c], sizeof(float) * 3);

		all_vertices[v_a].AddTriangle(tri);
		all_vertices[v_b].AddTriangle(tri);
		all_vertices[v_c].AddTriangle(tri);
	}

	int remaining_vertices;
	bool end = false;
	while (!end)
	{
		for (int i = 0; i < all_vertices.size(); i++)
		{
			bool erased = false;
			for (int j = i - 1; j >= 0; j--)
			{
				if ((math::float3)all_vertices[i].Vec() == all_vertices[j].Vec())
				{
					all_vertices[j].AddTriangles(all_vertices[i]);
					all_vertices.erase(all_vertices.begin() + i);
					erased = true;
					break;
				}
			}

			if (i < all_vertices.size())
				end = true;

			if (erased)
				break;
		}
	}

	root = new KDTNodeVertex(limits);

	if (all_vertices.size() > 0)
	{
		unsigned int num_subdivisions = 0;
		for (std::vector<Vertex>::const_iterator it = all_vertices.begin(); it != all_vertices.end(); ++it)
		{
			if (root->AddVertex(*it, num_subdivisions) == false)
			{
				LOG("Vertex: '%f, %f, %f' could not be added to KDT", (*it).X(), (*it).Y(), (*it).Z());
				return false;
			}
			num_subdivisions = 0;
		}
	}

	return true;
}

void KDTreeVertex::Draw() const
{
	root->Draw();
}

float KDTreeVertex::RayCollisionKDT(const LineSegment * ray) const
{
	float shortest_distance = ray->Length();
	if (root->RayCollisionKDT(ray, shortest_distance))
		LOG("Ray did not collide");
	return shortest_distance;
}