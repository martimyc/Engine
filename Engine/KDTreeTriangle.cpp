#include <functional>
#include <queue>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "Globals.h"
#include "KDTreeTriangle.h"

KDTNodeTriangle::KDTNodeTriangle(const math::vec min_point, const math::vec max_point) : partition_axis(NO_PARTITION), limits(min_point, max_point)
{
	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeTriangle::KDTNodeTriangle(const AABB& limits) : partition_axis(NO_PARTITION), limits(limits)
{
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

bool KDTNodeTriangle::SubDivide3D(const math::Triangle& new_triangle, unsigned int& num_subdivisions)
{
	if (++num_subdivisions >= MAX_SUBDIVISIONS)
	{
		LOG("Subdividing over limit");
		return false;
	}

	if (!AllSamePos(new_triangle))
	{
		float median_x = FindBestMedian(X, new_triangle);
		float median_y = FindBestMedian(Y, new_triangle);
		float median_z = FindBestMedian(Z, new_triangle);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (AddToCorrectChild(triangles[i], num_subdivisions) == false)
				return false;

			triangles[i].Infinite();
		}

		if (AddToCorrectChild(new_triangle, num_subdivisions) == false)
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

float KDTNodeTriangle::FindBestMedian(PARTITION_AXIS partition_axis, const math::Triangle& new_triangle) const
{
	switch (partition_axis)
	{
	case X: return FindBestMedianX(new_triangle);
	case Y: return FindBestMedianY(new_triangle);
	case Z: return FindBestMedianZ(new_triangle);
	default: LOG("Trying to find median of non-standard axsis");
	}
}

float KDTNodeTriangle::FindBestMedianX(const math::Triangle& new_triangle) const
{
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMaxPositionsX> max_queue;
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMinPositionsX> min_queue;

	bool repeated = false;

	max_queue.push(new_triangle);
	min_queue.push(new_triangle);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMaxPos().x == triangles[j].GetMaxPos().x)
				repeated = true;
		if (triangles[i].GetMaxPos().x == new_triangle.GetMaxPos().x)
			repeated = true;

		if (repeated == false)
			max_queue.push(triangles[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMinPos().x == triangles[j].GetMinPos().x)
				repeated = true;

		if (triangles[i].GetMinPos().x == new_triangle.GetMinPos().x)
			repeated = true;

		if (repeated == false)
			min_queue.push(triangles[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	math::Triangle current_max(max_queue.top());
	math::Triangle current_min(min_queue.top());

	bool unusable = false;

	while (max_queue.size() != 1 && min_queue.size() != 1)
	{
		if (current_max.GetMaxPos().x <= current_min.GetMinPos().x)
		{
			if (unusable)
				return current_max.GetMaxPos().x;

			float median = current_max.GetMaxPos().x;
			median += (current_min.GetMinPos().x - median) / 2.0f;
			return median;
		}
		else
		{
			unusable = true;

			min_queue.pop();
			if (current_max.GetMaxPos().x <= min_queue.top().GetMinPos().x)
			{
				float median = current_max.GetMaxPos().x;
				median += (min_queue.top().GetMinPos().x - median) / 2.0f;
				return median;
			}

			max_queue.pop();
			if (current_min.GetMinPos().x >= max_queue.top().GetMaxPos().x)
			{
				float median = max_queue.top().GetMaxPos().x;
				median += (current_min.GetMinPos().x - median) / 2.0f;
				return median;
			}

			current_max = max_queue.top();
			current_min = min_queue.top();
		}
	}

	LOG("Median could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		max_queue.push(triangles[i]);

	max_queue.push(new_triangle);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().x;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().x - fail) / 2.0f;

	return fail;
}

float KDTNodeTriangle::FindBestMedianY(const math::Triangle& new_triangle) const
{
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMaxPositionsY> max_queue;
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMinPositionsY> min_queue;

	bool repeated = false;

	max_queue.push(new_triangle);
	min_queue.push(new_triangle);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMaxPos().y == triangles[j].GetMaxPos().y)
				repeated = true;
		if (triangles[i].GetMaxPos().y == new_triangle.GetMaxPos().y)
			repeated = true;

		if (repeated == false)
			max_queue.push(triangles[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMinPos().y == triangles[j].GetMinPos().y)
				repeated = true;

		if (triangles[i].GetMinPos().y == new_triangle.GetMinPos().y)
			repeated = true;

		if (repeated == false)
			min_queue.push(triangles[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	math::Triangle current_max(max_queue.top());
	math::Triangle current_min(min_queue.top());

	bool unusable = false;

	while (max_queue.size() != 1 && min_queue.size() != 1)
	{
		if (current_max.GetMaxPos().y <= current_min.GetMinPos().y)
		{
			if (unusable)
				return current_max.GetMaxPos().y;

			float median = current_max.GetMaxPos().y;
			median += (current_min.GetMinPos().y - median) / 2.0f;
			return median;
		}
		else
		{
			unusable = true;

			min_queue.pop();
			if (current_max.GetMaxPos().y <= min_queue.top().GetMinPos().y)
			{
				float median = current_max.GetMaxPos().y;
				median += (min_queue.top().GetMinPos().y - median) / 2.0f;
				return median;
			}

			max_queue.pop();
			if (current_min.GetMinPos().y >= max_queue.top().GetMaxPos().y)
			{
				float median = max_queue.top().GetMaxPos().y;
				median += (current_min.GetMinPos().y - median) / 2.0f;
				return median;
			}

			current_max = max_queue.top();
			current_min = min_queue.top();
		}
	}

	LOG("Median could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		max_queue.push(triangles[i]);

	max_queue.push(new_triangle);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().y;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().y - fail) / 2.0f;

	return fail;
}

float KDTNodeTriangle::FindBestMedianZ(const math::Triangle& new_triangle) const
{
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMaxPositionsZ> max_queue;
	std::priority_queue<math::Triangle, std::vector<math::Triangle>, KDTT::CompareMinPositionsZ> min_queue;

	bool repeated = false;

	max_queue.push(new_triangle);
	min_queue.push(new_triangle);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMaxPos().z == triangles[j].GetMaxPos().z)
				repeated = true;
		if (triangles[i].GetMaxPos().z == new_triangle.GetMaxPos().z)
			repeated = true;

		if (repeated == false)
			max_queue.push(triangles[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (triangles[i].GetMinPos().z == triangles[j].GetMinPos().z)
				repeated = true;

		if (triangles[i].GetMinPos().z == new_triangle.GetMinPos().z)
			repeated = true;

		if (repeated == false)
			min_queue.push(triangles[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	math::Triangle current_max(max_queue.top());
	math::Triangle current_min(min_queue.top());

	bool unusable = false;

	while (max_queue.size() != 1 && min_queue.size() != 1)
	{
		if (current_max.GetMaxPos().z <= current_min.GetMinPos().z)
		{
			if (unusable)
				return current_max.GetMaxPos().z;

			float median = current_max.GetMaxPos().z;
			median += (current_min.GetMinPos().z - median) / 2.0f;
			return median;
		}
		else
		{
			unusable = true;

			min_queue.pop();
			if (current_max.GetMaxPos().z <= min_queue.top().GetMinPos().z)
			{
				float median = current_max.GetMaxPos().z;
				median += (min_queue.top().GetMinPos().z - median) / 2.0f;
				return median;
			}

			max_queue.pop();
			if (current_min.GetMinPos().z >= max_queue.top().GetMaxPos().z)
			{
				float median = max_queue.top().GetMaxPos().z;
				median += (current_min.GetMinPos().z - median) / 2.0f;
				return median;
			}

			current_max = max_queue.top();
			current_min = min_queue.top();
		}
	}

	LOG("Median could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		max_queue.push(triangles[i]);

	max_queue.push(new_triangle);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().z;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().z - fail) / 2.0f;

	return fail;
}

bool KDTNodeTriangle::AddTriangle(const math::Triangle& new_triangle, unsigned int& num_subdivisions)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (triangles[MAX_NUM_OBJECTS - 1].IsFinite())
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
				if (triangles[i].IsFinite() == false)
				{
					triangles[i] = new_triangle;
					ret = true;
					break;
				}
		}
		else
		{
			if (SubDivide3D(new_triangle, num_subdivisions) == false)
				return false;
			else
				ret = true;
		}
	}
	else
	{
		if (AddToCorrectChild(new_triangle, num_subdivisions) == false)
			return false;
		else
			ret = true;
	}

	return ret;
}

bool KDTNodeTriangle::AddToCorrectChild(const math::Triangle& new_triangle, unsigned int& num_subdivisions)
{
	bool ret = false;

	math::vec max_pos(new_triangle.GetMaxPos());
	math::vec min_pos(new_triangle.GetMinPos());

	if (min_pos[partition_axis] < median)
		if (childs[0]->AddTriangle(new_triangle, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (max_pos[partition_axis] > median)
		if (childs[1]->AddTriangle(new_triangle, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (ret == false)
		LOG("Gameobject does not intersect with any child");

	return ret;
}

bool KDTNodeTriangle::RemoveTriangle(const math::Triangle& new_triangle)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (triangles[i] == new_triangle)
			{
				triangles[i].Infinite();
				ret = true;
				ReArrange();
				break;
			}
	}
	else
		if (childs[0]->RemoveTriangle(new_triangle) || childs[1]->RemoveTriangle(new_triangle))
			ret = true;

	if (partition_axis == Z && Empty())
		if (childs != nullptr)
		{
			childs[0]->DeleteHirarchy();
			childs[1]->DeleteHirarchy();
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
	math::vec max_pos(new_triangle.GetMaxPos());
	math::vec min_pos(new_triangle.GetMinPos());

	if (max_pos.x < limits.minPoint.x || min_pos.x > limits.maxPoint.x)
		return false;
	if (max_pos.y < limits.minPoint.y || min_pos.y > limits.maxPoint.y)
		return false;
	if (max_pos.z < limits.minPoint.z || min_pos.z > limits.maxPoint.z)
		return false;

	return true;
}

bool KDTNodeTriangle::AllIn(const math::Triangle& new_triangle) const
{
	math::vec max_point(new_triangle.GetMaxPos());
	math::vec min_point(new_triangle.GetMinPos());

	if (limits.Contains(max_point) && limits.Contains(min_point)) //TODO sphere and oobb checks when those are done
		return true;
	return false;
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

bool KDTNodeTriangle::UpdateTriangle(const math::Triangle& updated_go)
{
	bool ret = false;

	if (partition_axis != NO_PARTITION)
	{

		if (childs[0]->AllIn(updated_go) == false || childs[1]->AllIn(updated_go) == false)
		{
			RemoveTriangle(updated_go);
			unsigned int num_subdivisions = 0;
			return AddToCorrectChild(updated_go, num_subdivisions);
		}
		else
		{
			if (childs[0]->UpdateTriangle(updated_go))
				ret = true;
			if (childs[1]->UpdateTriangle(updated_go))
				ret = true;
		}

		return ret;
	}
	else
		return true;
}

void KDTNodeTriangle::DeleteHirarchy()
{
	median = 0;
	partition_axis = NO_PARTITION;
	DELETE_PTR(childs[0]);
	DELETE_PTR(childs[1]);
}


bool KDTNodeTriangle::RayCollisionKDT(const LineSegment * ray, Triangle& triangle) const
{
	bool ret = false;
	if (partition_axis == NO_PARTITION)
	{
		if (triangles[0].IsFinite())
			return false;
		else
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (triangles[i].IsFinite() == true)
				{
					float shortest_dist = ray->Length();
					float distance = 0.0f;

					bool hit = false;

					if (ray->Intersects(triangles[i], &distance, nullptr))
					{
						if (shortest_dist > distance)
						{
							hit = true;
							shortest_dist = distance;
							triangle = triangles[i];
						}
					}

					if (hit)
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
			if (childs[0]->RayCollisionKDT(ray, triangle))
				ret = true;
			if (childs[1]->RayCollisionKDT(ray, triangle))
				ret = true;
		}
	}

	return ret;
}

KDTreeTriangle::KDTreeTriangle()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeTriangle(limits);
}

KDTreeTriangle::~KDTreeTriangle()
{
	delete root;
}

bool KDTreeTriangle::AddTriangles(const float*const new_vertices, const unsigned int*const new_indices, int num_indices)
{
	std::vector<math::Triangle> all_triangles;

	AABB limits;
	limits.SetNegativeInfinity();

	int num_triangles = num_indices / 3;

	for (int i = 0; i < num_triangles; i++)
	{
		math::Triangle tri;

		memcpy(&tri.a, &new_vertices[new_indices[i * 3]], sizeof(float) * 3);
		memcpy(&tri.b, &new_vertices[new_indices[i * 3 + 1]], sizeof(float) * 3);
		memcpy(&tri.c, &new_vertices[new_indices[i * 3 + 2]], sizeof(float) * 3);

		all_triangles.push_back(tri);
	}

	root = new KDTNodeTriangle(limits);

	if (all_triangles.size() > 0)
	{
		unsigned int num_subdivisions = 0;
		for (std::vector<math::Triangle>::const_iterator it = all_triangles.begin(); it != all_triangles.end(); ++it)
			if (root->AddTriangle(*it, num_subdivisions) == false)
			{
				LOG("Triangle: A:'%f, %f, %f'\nB:'%f, %f, %f'\nC:'%f, %f, %f'\n could not be added to KDT", (*it).a.x, (*it).a.y, (*it).a.z, (*it).b.x, (*it).b.y, (*it).b.z, (*it).c.x, (*it).c.y, (*it).c.z);
				return false;
			}
	}

	return true;
}

void KDTreeTriangle::Draw() const
{
	root->Draw();
}

bool KDTreeTriangle::RayCollisionKDT(const LineSegment * ray, Triangle& triangle) const
{
	return root->RayCollisionKDT(ray, triangle);
}

//Subdivide priority queue operators
bool KDTT::CompareMaxPositionsX::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMaxPos().x < v2.GetMaxPos().x;
}

bool KDTT::CompareMaxPositionsY::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMaxPos().y < v2.GetMaxPos().y;
}

bool KDTT::CompareMaxPositionsZ::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMaxPos().z < v2.GetMaxPos().z;
}

bool KDTT::CompareMinPositionsX::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMinPos().x > v2.GetMinPos().x;
}

bool KDTT::CompareMinPositionsY::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMinPos().y > v2.GetMinPos().y;
}

bool KDTT::CompareMinPositionsZ::operator()(const math::Triangle& v1, const math::Triangle& v2)
{
	return v1.GetMinPos().z > v2.GetMinPos().z;
}
