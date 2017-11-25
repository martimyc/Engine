#include <functional>
#include <queue>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\LineSegment.h"
#include "MathGeoLib\src\Geometry\Triangle.h"
#include "Globals.h"
#include "Vertex.h"
#include "KDTreeVertex.h"

KDTNodeVertex::KDTNodeVertex(const math::vec min_point, const math::vec max_point) : partition_axis(NO_PARTITION), limits(min_point, max_point)
{
	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeVertex::KDTNodeVertex(const AABB& limits) : partition_axis(NO_PARTITION), limits(limits)
{
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
		float median_x = FindBestMedian(X, new_vertex);
		float median_y = FindBestMedian(Y, new_vertex);
		float median_z = FindBestMedian(Z, new_vertex);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (AddToCorrectChild(vertices[i], num_subdivisions) == false)
				return false;

			vertices[i] = math::vec::inf;
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

float KDTNodeVertex::FindBestMedian(PARTITION_AXIS partition_axis, const Vertex& new_vertex) const
{
	switch (partition_axis)
	{
	case X: return FindBestMedianX(new_vertex);
	case Y: return FindBestMedianY(new_vertex);
	case Z: return FindBestMedianZ(new_vertex);
	default: LOG("Trying to find median of non-standard axsis");
	}
}

float KDTNodeVertex::FindBestMedianX(const Vertex& new_vertex) const
{
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMaxPositionsX> max_queue;
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMinPositionsX> min_queue;

	bool repeated = false;

	max_queue.push(new_vertex);
	min_queue.push(new_vertex);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMaxPos().x == vertices[j].GetMaxPos().x)
				repeated = true;
		if (vertices[i].GetMaxPos().x == new_vertex.GetMaxPos().x)
			repeated = true;

		if (repeated == false)
			max_queue.push(vertices[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMinPos().x == vertices[j].GetMinPos().x)
				repeated = true;

		if (vertices[i].GetMinPos().x == new_vertex.GetMinPos().x)
			repeated = true;

		if (repeated == false)
			min_queue.push(vertices[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	Vertex current_max(max_queue.top());
	Vertex current_min(min_queue.top());

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
		max_queue.push(vertices[i]);

	max_queue.push(new_vertex);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().x;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().x - fail) / 2.0f;

	return fail;
}

float KDTNodeVertex::FindBestMedianY(const Vertex& new_vertex) const
{
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMaxPositionsY> max_queue;
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMinPositionsY> min_queue;

	bool repeated = false;

	max_queue.push(new_vertex);
	min_queue.push(new_vertex);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMaxPos().y == vertices[j].GetMaxPos().y)
				repeated = true;
		if (vertices[i].GetMaxPos().y == new_vertex.GetMaxPos().y)
			repeated = true;

		if (repeated == false)
			max_queue.push(vertices[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMinPos().y == vertices[j].GetMinPos().y)
				repeated = true;

		if (vertices[i].GetMinPos().y == new_vertex.GetMinPos().y)
			repeated = true;

		if (repeated == false)
			min_queue.push(vertices[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	Vertex current_max(max_queue.top());
	Vertex current_min(min_queue.top());

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
		max_queue.push(vertices[i]);

	max_queue.push(new_vertex);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().y;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().y - fail) / 2.0f;

	return fail;
}

float KDTNodeVertex::FindBestMedianZ(const Vertex& new_vertex) const
{
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMaxPositionsZ> max_queue;
	std::priority_queue<Vertex, std::vector<Vertex>, KDTV::CompareMinPositionsZ> min_queue;

	bool repeated = false;

	max_queue.push(new_vertex);
	min_queue.push(new_vertex);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMaxPos().z == vertices[j].GetMaxPos().z)
				repeated = true;
		if (vertices[i].GetMaxPos().z == new_vertex.GetMaxPos().z)
			repeated = true;

		if (repeated == false)
			max_queue.push(vertices[i]);

		repeated == false;

		for (int j = i + 1; j < MAX_NUM_OBJECTS; j++)
			if (vertices[i].GetMinPos().z == vertices[j].GetMinPos().z)
				repeated = true;

		if (vertices[i].GetMinPos().z == new_vertex.GetMinPos().z)
			repeated = true;

		if (repeated == false)
			min_queue.push(vertices[i]);
	}

	for (int i = max_queue.size() / 2; i > 0; i--) //Empty biggest max & smallest min
	{
		max_queue.pop();
		min_queue.pop();
	}

	Vertex current_max(max_queue.top());
	Vertex current_min(min_queue.top());

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
		max_queue.push(vertices[i]);

	max_queue.push(new_vertex);

	for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
		max_queue.pop();

	float fail = max_queue.top().GetMaxPos().z;

	max_queue.pop();

	fail += (max_queue.top().GetMaxPos().z - fail) / 2.0f;

	return fail;
}

bool KDTNodeVertex::AddVertex(const Vertex& new_vertex, unsigned int& num_subdivisions)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (vertices[MAX_NUM_OBJECTS - 1] == nullptr)
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
				if (vertices[i] == math::float3::inf)
				{
					vertices[i] = new_vertex;
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

	math::vec max_pos(new_vertex.GetMaxPos());
	math::vec min_pos(new_vertex.GetMinPos());

	if (min_pos[partition_axis] < median)
		if (childs[0]->AddVertex(new_vertex, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (max_pos[partition_axis] > median)
		if (childs[1]->AddVertex(new_vertex, num_subdivisions) == false)
			return false;
		else
			ret = true;

	if (ret == false)
		LOG("Gameobject does not intersect with any child");

	return ret;
}

bool KDTNodeVertex::RemoveVertex(const Vertex& new_vertex)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (vertices[i] == new_vertex)
			{
				vertices[i] = math::float3::inf;
				ret = true;
				ReArrange();
				break;
			}
	}
	else
		if (childs[0]->RemoveVertex(new_vertex) || childs[1]->RemoveVertex(new_vertex))
			ret = true;

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
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (vertices[i] == math::float3::inf && vertices[i + 1] != nullptr)
		{
			vertices[i] = vertices[i + 1];
			i = 0;
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
		if (vertices[0] == math::vec::inf)
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if ((vertices[i] == math::float3::inf) == false)
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
	math::vec max_pos(new_vertex.GetMaxPos());
	math::vec min_pos(new_vertex.GetMinPos());

	if (max_pos.x < limits.minPoint.x || min_pos.x > limits.maxPoint.x)
		return false;
	if (max_pos.y < limits.minPoint.y || min_pos.y > limits.maxPoint.y)
		return false;
	if (max_pos.z < limits.minPoint.z || min_pos.z > limits.maxPoint.z)
		return false;

	return true;
}

bool KDTNodeVertex::AllIn(const Vertex& new_vertex) const
{
	math::vec max_point(new_vertex.GetMaxPos());
	math::vec min_point(new_vertex.GetMinPos());

	if (limits.Contains(max_point) && limits.Contains(min_point)) //TODO sphere and oobb checks when those are done
		return true;
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
		glPointSize(25.0f);

		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_POINTS);
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (vertices[i] != math::float3::inf)
			{
				math::vec position = vertices[i].vertex;
				glVertex3f(position.x, position.y, position.z);
			}
			else
				break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

bool KDTNodeVertex::AllSamePos(const Vertex& new_vertex) const
{
	math::vec position = new_vertex.vertex;

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (position.x != vertices[i].vertex.x || position.y != vertices[i].vertex.y || position.z != vertices[i].vertex.z)
			return false;
	return true;
}

bool KDTNodeVertex::UpdateGO(const Vertex& updated_go)
{
	bool ret = false;

	if (partition_axis != NO_PARTITION)
	{

		if (childs[0]->AllIn(updated_go) == false || childs[1]->AllIn(updated_go) == false)
		{
			RemoveVertex(updated_go);
			unsigned int num_subdivisions = 0;
			return AddToCorrectChild(updated_go, num_subdivisions);
		}
		else
		{
			if (childs[0]->UpdateGO(updated_go))
				ret = true;
			if (childs[1]->UpdateGO(updated_go))
				ret = true;
		}

		return ret;
	}
	else
		return true;
}

void KDTNodeVertex::DeleteHirarchy()
{
	median = 0;
	partition_axis = NO_PARTITION;
	DELETE_PTR(childs[0]);
	DELETE_PTR(childs[1]);
}


bool KDTNodeVertex::RayCollisionKDT(const LineSegment * ray, Triangle& triangle) const
{
	bool ret = false;
	if (partition_axis == NO_PARTITION)
	{
		if (vertices[0] == math::float3::inf)
			return false;
		else
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (vertices[i] != math::float3::inf)
				{
					if (vertices[i].CheckCollision(ray, triangle))
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

KDTreeVertex::KDTreeVertex()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeVertex(limits);
}

KDTreeVertex::~KDTreeVertex()
{
	delete root;
}

bool KDTreeVertex::AddVertices(const float*const new_vertices, int num_vertices, const unsigned int*const new_indices, int num_indices)
{
	std::vector<Vertex> all_vertices;

	AABB limits;
	limits.SetNegativeInfinity();

	int i = 0;

	for (; i < num_vertices; i++)
	{
		Vertex vertex(&new_vertices[i * 3]);
		all_vertices.push_back(vertex);
		limits.Enclose(vertex.vertex);
	}

	int num_triangles = num_indices / 3;

	for (i = 0; i < num_triangles; i++)
	{
		Vertex v1 = all_vertices[new_indices[i * 3]];
		Vertex v2 = all_vertices[new_indices[i * 3 + 1]];
		Vertex v3 = all_vertices[new_indices[i * 3 + 2]];

		math::Triangle* triangle = new math::Triangle(v1.vertex, v2.vertex, v3.vertex);

		v1.AddTriangle(triangle);
		v2.AddTriangle(triangle);
		v3.AddTriangle(triangle);
	}

	root = new KDTNodeVertex(limits);

	if (all_vertices.size() > 0)
	{
		unsigned int num_subdivisions = 0;
		for (std::vector<Vertex>::const_iterator it = all_vertices.begin(); it != all_vertices.end(); ++it)
			if (root->AddVertex(*it, num_subdivisions) == false)
			{
				LOG("Vertex: '%f, %f, %f' could not be added to KDT", (*it).vertex.x, (*it).vertex.y, (*it).vertex.z);
				return false;
			}
	}

	return true;
}

void KDTreeVertex::Draw() const
{
	root->Draw();
}

bool KDTreeVertex::RayCollisionKDT(const LineSegment * ray, Triangle& triangle) const
{
	return root->RayCollisionKDT(ray, triangle);
}

//Subdivide priority queue operators
bool KDTV::CompareMaxPositionsX::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMaxPos().x < v2.GetMaxPos().x;
}

bool KDTV::CompareMaxPositionsY::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMaxPos().y < v2.GetMaxPos().y;
}

bool KDTV::CompareMaxPositionsZ::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMaxPos().z < v2.GetMaxPos().z;
}

bool KDTV::CompareMinPositionsX::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMinPos().x > v2.GetMinPos().x;
}

bool KDTV::CompareMinPositionsY::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMinPos().y > v2.GetMinPos().y;
}

bool KDTV::CompareMinPositionsZ::operator()(const Vertex& v1, const Vertex& v2)
{
	return v1.GetMinPos().z > v2.GetMinPos().z;
}
