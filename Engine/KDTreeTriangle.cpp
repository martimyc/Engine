#include <functional>
#include <queue>
#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "MathGeoLib\src\Geometry\Triangle.h"
#include "Globals.h"
#include "KDTreeTriangle.h"

KDTNodeTriangle::KDTNodeTriangle(const math::vec min_point, const math::vec max_point) : partition_axis(NO_PARTITION), limits(min_point, max_point), max_vec(math::vec::nan), min_vec(math::vec::nan)
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		triangles[i].Infinite();

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeTriangle::KDTNodeTriangle(const AABB& limits) : partition_axis(NO_PARTITION), limits(limits), max_vec(math::vec::nan), min_vec(math::vec::nan)
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		triangles[i].Infinite();

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeTriangle::~KDTNodeTriangle() //Dont think deleting gameobjects through tree is best
{
	if (childs[0] != nullptr)
	{
		delete childs[0];
		delete childs[1];
	}
}

bool KDTNodeTriangle::SubDivide3D(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes)
{
	if (++num_subdivisions >= MAX_SUBDIVISIONS)
	{
		LOG("Subdividing over limit");
		return false;
	}

	num_nodes += 2 + 4 + 8;
	bytes += sizeof(KDTNodeTriangle) * (2 + 4 + 8);

	if (!AllSamePos(new_triangle))
	{
		float median_x = FindBestMedianX(new_triangle);
		float median_y = FindBestMedianY(new_triangle);
		float median_z = FindBestMedianZ(new_triangle);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (AddToCorrectChild(triangles[i], num_subdivisions, num_nodes, bytes) == false)
				return false;

			triangles[i].Infinite();
		}

		if (AddToCorrectChild(new_triangle, num_subdivisions, num_nodes, bytes) == false)
			return false;

		return true;
	}
	else
		LOG("Trying to subdivide with all objects in the same place");
	return false;
}

void KDTNodeTriangle::SubDivideChilds(PARTITION_AXIS partition_axis, float median)
{
	if (childs[0] != nullptr)
	{
		childs[0]->SubDivide(partition_axis, median);
		childs[1]->SubDivide(partition_axis, median);
	}
	else
		LOG("No childs to subdivide");
}

void KDTNodeTriangle::SubDivide(PARTITION_AXIS partition_axis, float median)
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

		childs[0] = new KDTNodeTriangle(min_point, max_point);

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

		childs[1] = new KDTNodeTriangle(min_point, max_point);
	}
	else
		LOG("Trying to subdivide node with existing childs");
}

float KDTNodeTriangle::FindBestMedianX(const math::Triangle& new_triangle) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_triangle.CenterPoint().x);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (triangles[i].CenterPoint().x == new_triangle.CenterPoint().x)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (triangles[i].CenterPoint().x == triangles[j].CenterPoint().x)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			queue.push(triangles[i].CenterPoint().x);
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max_vec & smallest min_vec
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

float KDTNodeTriangle::FindBestMedianY(const math::Triangle& new_triangle) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_triangle.CenterPoint().y);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (triangles[i].CenterPoint().y == new_triangle.CenterPoint().y)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (triangles[i].CenterPoint().y == triangles[j].CenterPoint().y)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			queue.push(triangles[i].CenterPoint().y);
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max_vec & smallest min_vec
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

float KDTNodeTriangle::FindBestMedianZ(const math::Triangle& new_triangle) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> queue;

	bool repeated = false;

	queue.push(new_triangle.CenterPoint().z);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (triangles[i].CenterPoint().z == new_triangle.CenterPoint().z)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (triangles[i].CenterPoint().z == triangles[j].CenterPoint().z)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			queue.push(triangles[i].CenterPoint().z);
	}

	if (queue.size() >= 2)
	{
		if (queue.size() > 2)
		{
			for (int i = queue.size() / 2 - 1; i > 0; i--) //Empty biggest max_vec & smallest min_vec
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

float KDTNodeTriangle::GetDistance() const
{
	math::vec center (limits.CenterPoint());
	float distance = abs(max_vec.x - center.x);

	if (abs(min_vec.x - center.x) > distance)
		distance = abs(min_vec.x - center.x);

	if (abs(max_vec.y - center.y) > distance)
		distance = abs(max_vec.y - center.y);
	if (abs(min_vec.y - center.y) > distance)
		distance = abs(min_vec.y - center.y);

	if (abs(max_vec.z - center.z) > distance)
		distance = abs(max_vec.z - center.z);
	if (abs(min_vec.z - center.z) > distance)
		distance = abs(min_vec.z - center.z);

	return distance;
}

bool KDTNodeTriangle::AddTriangle(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (IsIn(new_triangle))
		{
			LOG("Adding repeated triangle");
			ret = true;
		}
		else
		{
			if (triangles[MAX_NUM_OBJECTS - 1].IsFinite() == false)
			{
				for (int i = 0; i < MAX_NUM_OBJECTS; i++)
					if (triangles[i].IsFinite() == false)
					{
						triangles[i] = new_triangle;
						math::vec v(new_triangle.GetMaxPos());
						UpdateMax(v);
						v = new_triangle.GetMinPos();
						UpdateMin(v);
						ret = true;
						break;
					}
			}
			else
			{
				if (SubDivide3D(new_triangle, num_subdivisions, num_nodes, bytes) == false)
					return false;
				else
					ret = true;
			}
		}
	}
	else
	{
		if (AddToCorrectChild(new_triangle, num_subdivisions, num_nodes, bytes) == false)
			return false;
		
		ret = true;
		UpdateMax();
		UpdateMin();
	}

	return ret;
}

bool KDTNodeTriangle::AddToCorrectChild(const math::Triangle& new_triangle, unsigned int& num_subdivisions, unsigned int& num_nodes, unsigned int& bytes)
{
	bool ret = false;

	float pos = 0.0f;

	if (new_triangle.CenterPoint()[partition_axis] <= median)
		if (childs[0]->AddTriangle(new_triangle, num_subdivisions, num_nodes, bytes) == false)
			return false;
		else
			ret = true;

	if (new_triangle.CenterPoint()[partition_axis] > median)
		if (childs[1]->AddTriangle(new_triangle, num_subdivisions, num_nodes, bytes) == false)
			return false;
		else
			ret = true;

	if (ret == false)
		LOG("math::Triangle is not in any child");

	return ret;
}

void KDTNodeTriangle::UpdateMax( math::vec vec)
{
	if (max_vec.IsFinite() == false)
		max_vec = vec;

	if (vec.x > max_vec.x)
		max_vec.x = vec.x;
	if (vec.y > max_vec.y)
		max_vec.y = vec.y;
	if (vec.z > max_vec.z)
		max_vec.z = vec.z;
}

void KDTNodeTriangle::UpdateMin( math::vec vec)
{
	if (min_vec.IsFinite() == false)
		min_vec = vec;

	if (vec.x < min_vec.x)
		min_vec.x = vec.x;
	if (vec.y < min_vec.y)
		min_vec.y = vec.y;
	if (vec.z < min_vec.z)
		min_vec.z = vec.z;
}

void KDTNodeTriangle::UpdateMax()
{
	if (partition_axis == NO_PARTITION)
	{
		max_vec = triangles[0].GetMaxPos();
		for (int i = 1; i < MAX_NUM_OBJECTS; i++)
			if (triangles[i].IsFinite())
				UpdateMax(triangles[i].GetMaxPos());
			else
				break;
	}
	else
	{
		math::vec v(childs[0]->GetMax());
		UpdateMax(v);
		v = childs[1]->GetMax();
		UpdateMax(v);
	}
}

void KDTNodeTriangle::UpdateMin()
{
	if (partition_axis == NO_PARTITION)
	{
		max_vec = triangles[0].GetMinPos();
		for (int i = 1; i < MAX_NUM_OBJECTS; i++)
			if (triangles[i].IsFinite())
				UpdateMin(triangles[i].GetMinPos());
			else
				break;
	}
	else
	{
		UpdateMin(childs[0]->GetMin());
		UpdateMin(childs[1]->GetMin());
	}
}

const math::vec& KDTNodeTriangle::GetMax() const
{
	return max_vec;
}

const math::vec& KDTNodeTriangle::GetMin() const
{
	return min_vec;
}

math::AABB KDTNodeTriangle::GetMaxMinAABB() const
{
	return math::AABB(min_vec, max_vec);
}

bool KDTNodeTriangle::RemoveTriangle(const math::Triangle& new_triangle, unsigned int& num_nodes, unsigned int& bytes)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (triangles[i] == new_triangle)
			{
				math::vec v_max(triangles[i].GetMaxPos());
				math::vec v_min(triangles[i].GetMinPos());

				triangles[i].Infinite();
				ret = true;
				ReArrange();

				if (max_vec.x == v_max.x || max_vec.y == v_max.y || max_vec.z == v_max.z)
					UpdateMax();
				if (min_vec.x == v_min.x || min_vec.y == v_min.y || min_vec.z == v_min.z)
					UpdateMin();

				break;
			}
	}
	else
	{
		if(childs[0]->WithinLimits(new_triangle.CenterPoint()))
			if (childs[0]->RemoveTriangle(new_triangle, num_nodes, bytes))
			{
				ret = true;
				UpdateMax();
				UpdateMin();
			}

		if (childs[1]->WithinLimits(new_triangle.CenterPoint()))
			if (childs[1]->RemoveTriangle(new_triangle, num_nodes, bytes))
			{
				ret = true;
				UpdateMax();
				UpdateMin();
			}
	}

	if (partition_axis == Z && Empty())
		if (childs != nullptr)
		{
			childs[0]->DeleteHirarchy();
			childs[1]->DeleteHirarchy();

			num_nodes -= 2 + 4 + 8;
			bytes -= sizeof(KDTNodeTriangle) * (2 + 4 + 8);
		}

	return ret;
}

void KDTNodeTriangle::ReArrange()
{
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (triangles[i].IsFinite() == false && triangles[i + 1].IsFinite() == true)
		{
			triangles[i] = triangles[i + 1];
			i = 0;
			break;
		}
}

bool KDTNodeTriangle::Empty() const
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

void KDTNodeTriangle::GetTriangles(std::vector<math::Triangle>& vec) const
{
	if (partition_axis != NO_PARTITION)
	{
		childs[0]->GetTriangles(vec);
		childs[1]->GetTriangles(vec);
	}
	else
	{
		if (triangles[0].IsFinite() == false)
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (triangles[i].IsFinite() == true)
				{
					bool repeated = false;
					for (std::vector<math::Triangle>::const_iterator it = vec.begin(); it != vec.end(); ++it)
						if ((*it) == triangles[i])
							repeated = true;
					if (!repeated)
						vec.push_back(triangles[i]);
				}
				else
					break;
			}
		}
	}
}

bool KDTNodeTriangle::IsIn(const math::Triangle& new_triangle) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (triangles[i].IsFinite())
		{
			if (triangles[i] == new_triangle)
				return true;
		}
		else
			break;
	return false;
}

bool KDTNodeTriangle::WithinLimits(const math::vec& vec) const
{
	return limits.Contains(vec);
}

bool KDTNodeTriangle::Intersects(const math::LineSegment * ray) const
{
	return ray->Distance(limits.CenterPoint()) <= GetDistance();
}

void KDTNodeTriangle::Draw() const
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
			if (triangles[i].IsFinite() == true)
				triangles[i].Draw(0.0f, 1.0f, 1.0f, 1.0f);
			else
				break;
		}
	}
}

bool KDTNodeTriangle::AllSamePos(const math::Triangle& new_triangle) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if ((new_triangle == triangles[i]) == false)
			return false;
	return true;
}

void KDTNodeTriangle::DeleteHirarchy()
{
	median = 0;
	partition_axis = NO_PARTITION;
	DELETE_PTR(childs[0]);
	DELETE_PTR(childs[1]);
}


bool KDTNodeTriangle::RayCollisionKDT(const LineSegment * ray, float& shortest_distance, unsigned int& num_checks) const
{
	bool ret = false;
	if (partition_axis == NO_PARTITION)
	{
		if (triangles[0].IsFinite() == false)
			return false;
		else
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (triangles[i].IsFinite() == true)
				{
					num_checks++;
					float distance = 0.0f;

					if (ray->Intersects(triangles[i], &distance, nullptr))
					{
						if (shortest_distance > distance)
						{
							ret = true;
							shortest_distance = distance;
						}
					}
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
			if (ray->Intersects(math::AABB(childs[0]->GetMaxMinAABB())))
			{
				if (childs[0]->RayCollisionKDT(ray, shortest_distance, num_checks))
					ret = true;
			}
			
			if (ray->Intersects(math::AABB(childs[1]->GetMaxMinAABB())))
			{
				if (childs[1]->RayCollisionKDT(ray, shortest_distance, num_checks))
					ret = true;
			}
		}
	}

	return ret;
}

bool KDTNodeTriangle::Inside(const math::Triangle & new_triangle) const
{
	if (new_triangle.CenterPoint().x < limits.MinX())
		return false;
	if (new_triangle.CenterPoint().x > limits.MaxX())
		return false;

	if (new_triangle.CenterPoint().y < limits.MinY())
		return false;
	if (new_triangle.CenterPoint().y > limits.MaxY())
		return false;

	if (new_triangle.CenterPoint().z < limits.MinZ())
		return false;
	if (new_triangle.CenterPoint().z > limits.MaxZ())
		return false;

	return true;
}

bool KDTreeTriangle::ReCalculate(const math::Triangle & new_triangle)
{
	std::vector<math::Triangle> all_triangles;

	root->GetTriangles(all_triangles);

	delete root;

	AABB limits;
	limits.SetNegativeInfinity();

	limits.Enclose(new_triangle.GetMaxPos());
	limits.Enclose(new_triangle.GetMinPos());

	if (all_triangles.size() > 0)
	{
		for (std::vector<math::Triangle>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
		{
			limits.Enclose(it->GetMaxPos());
			limits.Enclose(it->GetMinPos());
		}
	}

	root = new KDTNodeTriangle(limits);

	unsigned int num_subdivisions = 0;
	if (root->AddTriangle(new_triangle, num_subdivisions, num_nodes, bytes) == false)
	{
		LOG("New triangle:\nA:'%f, %f, %f'\nB:'%f, %f, %f'\nC:'%f, %f, %f'\nCould not be added to KDT", new_triangle.a.x, new_triangle.a.y, new_triangle.a.z, new_triangle.b.x, new_triangle.b.y, new_triangle.b.z, new_triangle.c.x, new_triangle.c.y, new_triangle.c.z);
		return false;
	}

	if (all_triangles.size() > 0)
	{
		for (std::vector<math::Triangle>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
		{
			num_subdivisions = 0;
			if (root->AddTriangle(*it, num_subdivisions, num_nodes, bytes) == false)
			{
				LOG("Triangle:\nA:'%f, %f, %f'\nB:'%f, %f, %f'\nC:'%f, %f, %f'\nCould not be re-added to KDT", (*it).a.x, (*it).a.y, (*it).a.z, (*it).b.x, (*it).b.y, (*it).b.z, (*it).c.x, (*it).c.y, (*it).c.z);
				return false;
			}
		}
	}

	return true;
}

KDTreeTriangle::KDTreeTriangle() : num_nodes(1), bytes(sizeof(KDTreeTriangle)), draw(false)
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeTriangle(limits);
}

KDTreeTriangle::~KDTreeTriangle()
{
	delete root;
}

bool KDTreeTriangle::AddTriangle(const math::Triangle& new_triangle)
{
	if (root->Inside(new_triangle))
	{
		unsigned int num_subdivisions = 0;
		if (root->AddTriangle(new_triangle, num_subdivisions, num_nodes, bytes) == false)
			return false;
	}
	else
		if (ReCalculate(new_triangle) == false)
			return false;

	bytes += sizeof(math::Triangle);
	return true;
}

bool KDTreeTriangle::RemoveTriangle(const math::Triangle& new_triangle)
{
	if (root->RemoveTriangle(new_triangle, num_nodes, bytes))
	{
		bytes -= sizeof(math::Triangle);
		return true;
	}
	return false;
}

bool KDTreeTriangle::AddTriangles(const float*const new_vertices, const unsigned int*const new_indices, int num_indices, const math::vec& max_vec, const math::vec& min_vec)
{
	std::vector<math::Triangle> all_triangles;

	AABB limits(min_vec,max_vec);

	int num_triangles = num_indices / 3;
	all_triangles.reserve(num_triangles);

	for (int i = 0; i < num_triangles; i++)
	{
		unsigned int v_a = new_indices[i * 3];
		unsigned int v_b = new_indices[i * 3 + 1];
		unsigned int v_c = new_indices[i * 3 + 2];

		Triangle tri;

		memcpy(&tri.a, &new_vertices[v_a * 3], sizeof(float) * 3);
		memcpy(&tri.b, &new_vertices[v_b * 3], sizeof(float) * 3);
		memcpy(&tri.c, &new_vertices[v_c * 3], sizeof(float) * 3);

		all_triangles.push_back(tri);

		//all_triangles.push_back((math::vec)new_vertices[v_a], (math::vec)new_vertices[v_b], (math::vec)new_vertices[v_c]);
	}

	root = new KDTNodeTriangle(limits);

	if (all_triangles.size() > 0)
	{
		unsigned int num_subdivisions = 0;
		for (std::vector<math::Triangle>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
		{
			if (root->AddTriangle(*it, num_subdivisions, num_nodes, bytes) == false)
			{
				LOG("Triangle:\nA:'%f, %f, %f'\nB:'%f, %f, %f'\nC:'%f, %f, %f'\nCould not be added to KDT", (*it).a.x, (*it).a.y, (*it).a.z, (*it).b.x, (*it).b.y, (*it).b.z, (*it).c.x, (*it).c.y, (*it).c.z);
				return false;
			}
			num_subdivisions = 0;
		}
	}

	return true;
}

void KDTreeTriangle::Draw() const
{
	if(draw)
		root->Draw();
}

float KDTreeTriangle::RayCollisionKDT(const LineSegment * ray, unsigned int& num_checks) const
{
	float shortest_distance = ray->Length();
	if (root->RayCollisionKDT(ray, shortest_distance, num_checks) == false)
		LOG("Ray did not collide");
	return shortest_distance;
}

void KDTreeTriangle::Inspector()
{
	ImGui::Checkbox("Draw", &draw);

	ImGui::Text("Number of Nodes: %i", num_nodes);
	ImGui::Text("Size: %i MB", bytes / 1024);
}
