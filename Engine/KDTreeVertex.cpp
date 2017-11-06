#include <functional>
#include <queue>
#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Mesh.h"
#include "KDTreeVertex.h"

KDTNodeVertex::KDTNodeVertex(const math::vec min_point, const math::vec max_point) : partition_axis(NO_PARTITION)
{
	limits = new AABB(min_point, max_point);
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		vertices[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeVertex::KDTNodeVertex(const AABB& limits) : partition_axis(NO_PARTITION)
{
	this->limits = new AABB(limits);
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		vertices[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeVertex::~KDTNodeVertex()
{
	delete limits;

	if (childs[0] != nullptr)
		delete[] childs;

	//Dont think deleting gameobjects through tree is best
}

bool KDTNodeVertex::SubDivide3D(const Geo::Vertex* new_vertex)
{
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
			math::vec vect(vec::zero);
			vect[0] = 3;
			if ((*vertices[i])[partition_axis] <= median)
				if (childs[0]->AddVertex(vertices[i]))
					return false;

			if ((*vertices[i])[partition_axis] > median)
				if (childs[1]->AddVertex(vertices[i]))
					return false;


			vertices[i] = nullptr;
		}
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

		math::vec min_point(limits->minPoint);
		math::vec max_point;

		switch (partition_axis)
		{
		case X:
			max_point.x = median;
			max_point.y = limits->maxPoint.y;
			max_point.z = limits->maxPoint.z;
			break;
		case Y:
			max_point.x = limits->maxPoint.x;
			max_point.y = median;
			max_point.z = limits->maxPoint.z;
			break;
		case Z:
			max_point.x = limits->maxPoint.x;
			max_point.y = limits->maxPoint.y;
			max_point.z = median;
			break;
		default:
			LOG("Non standard partition axis to subdivide");
			break;
		}

		childs[0] = new KDTNodeVertex(min_point, max_point);

		max_point = limits->maxPoint;

		switch (partition_axis)
		{
		case X:
			min_point.x = median;
			min_point.y = limits->minPoint.y;
			min_point.z = limits->minPoint.z;
			break;
		case Y:
			min_point.x = limits->minPoint.x;
			min_point.y = median;
			min_point.z = limits->minPoint.z;
			break;
		case Z:
			min_point.x = limits->minPoint.x;
			min_point.y = limits->minPoint.y;
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

float KDTNodeVertex::FindBestMedian(PARTITION_AXIS partition_axis, const Geo::Vertex* new_vertex) const
{
	std::priority_queue <float, std::vector<float>, std::greater<float>> queue;

	bool all_equal = true;

	queue.push((*new_vertex)[partition_axis]);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (vertices[i] != nullptr)
			queue.push((*vertices[i])[partition_axis]);
		else
			break;

	if (queue.size() % 2 != 0)
	{
		for (unsigned int to_pop = 0; to_pop < queue.size() / 2; to_pop++)
			queue.pop();
		return queue.top();
	}
	else
	{
		for (unsigned int to_pop = 0; to_pop < queue.size() / 2 - 1; to_pop++)
			queue.pop();
		float first = queue.top();

		queue.pop();
		float second = queue.top();

		return (first + second) / 2;
	}
}

bool KDTNodeVertex::AddVertex(const Geo::Vertex * new_vertex)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (vertices[MAX_NUM_OBJECTS - 1] == nullptr)
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
				if (vertices[i] == nullptr)
				{
					vertices[i] = new_vertex;
					ret = true;
					break;
				}
		}
		else
		{
			if (SubDivide3D(new_vertex) == false)
				return false;
			if (AddToCorrectChild(new_vertex) == false)
				return false;
		}
	}
	else
		if (AddToCorrectChild(new_vertex) == false)
			return false;

	return ret;
}

bool KDTNodeVertex::AddToCorrectChild(const Geo::Vertex * new_vertex)
{
	bool ret = false;

	if ((*new_vertex)[partition_axis] <= median)
	{
		if (childs[0]->AddVertex(new_vertex) == false)
			return false;
		else
			ret = true;
	}

	if ((*new_vertex)[partition_axis] > median)
	{
		if (childs[1]->AddVertex(new_vertex) == false)
			return false;
		else
			ret = true;
	}

	if (ret == false)
		LOG("Vertex does not fit inside any child");

	return ret;
}

bool KDTNodeVertex::RemoveVertex(const Geo::Vertex * new_vertex)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (vertices[i] == new_vertex)
			{
				vertices[i] = nullptr;
				ret = true;
				ReArrange();
				break;
			}
	}
	else
		if (childs[0]->RemoveVertex(new_vertex) || childs[1]->RemoveVertex(new_vertex))
			ret = true;

	if (partition_axis == Z && Empty())
		delete[] childs;

	return ret;
}

void KDTNodeVertex::ReArrange()
{
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (vertices[i] == nullptr && vertices[i + 1] != nullptr)
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

void KDTNodeVertex::GetVertices(std::vector<const Geo::Vertex*>& vec) const
{
	if (partition_axis != NO_PARTITION)
	{
		childs[0]->GetVertices(vec);
		childs[1]->GetVertices(vec);
	}
	else
	{
		if (vertices[0] != nullptr)
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (vertices[i] != nullptr)
					vec.push_back(vertices[i]);
				else
					break;
			}
	}
}

bool KDTNodeVertex::IsIn(const Geo::Vertex * new_vertex) const
{
	math::vec position(new_vertex->MGLVec());

	if (position.x < limits->minPoint.x || position.x > limits->maxPoint.x)
		return false;
	if (position.y < limits->minPoint.y || position.y > limits->maxPoint.y)
		return false;
	if (position.z < limits->minPoint.z || position.z > limits->maxPoint.z)
		return false;

	return true;
}

void KDTNodeVertex::Draw() const
{
	if (partition_axis != NO_PARTITION)
	{
		if (partition_axis == X)
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		if (partition_axis == Y)
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		if (partition_axis == Z)
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);

		switch (partition_axis)
		{
		case X:
			glVertex3f(median, limits->maxPoint.y, limits->maxPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->minPoint.z);

			glVertex3f(median, limits->maxPoint.y, limits->maxPoint.z);
			glVertex3f(median, limits->minPoint.y, limits->maxPoint.z);

			glVertex3f(median, limits->minPoint.y, limits->minPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->minPoint.z);

			glVertex3f(median, limits->minPoint.y, limits->minPoint.z);
			glVertex3f(median, limits->minPoint.y, limits->maxPoint.z);

			glVertex3f(median, limits->minPoint.y, limits->minPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->maxPoint.z);

			glVertex3f(median, limits->minPoint.y, limits->maxPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->minPoint.z);
			break;
		case Y:
			glVertex3f(limits->maxPoint.x, median, limits->maxPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->minPoint.z);

			glVertex3f(limits->maxPoint.x, median, limits->maxPoint.z);
			glVertex3f(limits->minPoint.x, median, limits->maxPoint.z);

			glVertex3f(limits->minPoint.x, median, limits->minPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->minPoint.z);

			glVertex3f(limits->minPoint.x, median, limits->minPoint.z);
			glVertex3f(limits->minPoint.x, median, limits->maxPoint.z);

			glVertex3f(limits->minPoint.x, median, limits->minPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->maxPoint.z);

			glVertex3f(limits->minPoint.x, median, limits->maxPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->minPoint.z);
			break;
		case Z:
			glVertex3f(limits->maxPoint.x, limits->maxPoint.y, median);
			glVertex3f(limits->maxPoint.x, limits->minPoint.y, median);

			glVertex3f(limits->maxPoint.x, limits->maxPoint.y, median);
			glVertex3f(limits->minPoint.x, limits->maxPoint.y, median);

			glVertex3f(limits->minPoint.x, limits->minPoint.y, median);
			glVertex3f(limits->maxPoint.x, limits->minPoint.y, median);

			glVertex3f(limits->minPoint.x, limits->minPoint.y, median);
			glVertex3f(limits->minPoint.x, limits->maxPoint.y, median);

			glVertex3f(limits->minPoint.x, limits->minPoint.y, median);
			glVertex3f(limits->maxPoint.x, limits->maxPoint.y, median);

			glVertex3f(limits->minPoint.x, limits->maxPoint.y, median);
			glVertex3f(limits->maxPoint.x, limits->minPoint.y, median);
			break;
		default:
			break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

		glBegin(GL_POINTS);

		switch (partition_axis)
		{
		case X:
			glVertex3f(median, limits->minPoint.y, limits->minPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->maxPoint.z);

			glVertex3f(median, limits->minPoint.y, limits->maxPoint.z);
			glVertex3f(median, limits->maxPoint.y, limits->minPoint.z);
			break;
		case Y:
			glVertex3f(limits->minPoint.x, median, limits->minPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->maxPoint.z);

			glVertex3f(limits->minPoint.x, median, limits->maxPoint.z);
			glVertex3f(limits->maxPoint.x, median, limits->minPoint.z);
			break;
		case Z:
			glVertex3f(limits->maxPoint.x, limits->maxPoint.y, median);
			glVertex3f(limits->maxPoint.x, limits->minPoint.y, median);

			glVertex3f(limits->minPoint.x, limits->minPoint.y, median);
			glVertex3f(limits->minPoint.x, limits->maxPoint.y, median);
			break;
		default:
			break;
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (vertices[i] != nullptr)
				glVertex3f(vertices[i]->x, vertices[i]->y, vertices[i]->z);
			else
				break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

bool KDTNodeVertex::AllSamePos(const Geo::Vertex * new_vertex) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (new_vertex->x != vertices[i]->x || new_vertex->y != vertices[i]->y || new_vertex->z != vertices[i]->z)
			return false;
	return true;
}

KDTreeVertex::KDTreeVertex()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeVertex(limits);
}

KDTreeVertex::~KDTreeVertex()
{}

bool KDTreeVertex::ReCalculate(Geo::Vertex* new_vertex)
{
	std::vector<const Geo::Vertex*> all_vertices;

	root->GetVertices(all_vertices);

	delete root;

	AABB limits;
	limits.SetNegativeInfinity();

	limits.Enclose(&new_vertex->MGLVec(), 1);

	if (all_vertices.size() > 0)
	{
		for (std::vector<const Geo::Vertex*>::const_iterator it = all_vertices.begin(); it != all_vertices.end(); ++it)
			limits.Enclose(&(*it)->MGLVec(), 1);
	}

	root = new KDTNodeVertex(limits);

	if (root->AddVertex(new_vertex) == false)
	{
		LOG("New Vertex: '%f, %f, %f' could not be added to KDT after recalculating", new_vertex->x, new_vertex->y, new_vertex->z);
		return false;
	}

	if (all_vertices.size() > 0)
	{
		for (std::vector<const Geo::Vertex*>::const_iterator it = all_vertices.begin(); it != all_vertices.end(); ++it)
			if (root->AddVertex(*it) == false)
			{
				LOG("Vertex: '%f, %f, %f' could not be re-added to KDT after recalculating", (*it)->x, (*it)->y, (*it)->z);
				return false;
			}
	}

	return true;
}

bool KDTreeVertex::AddVertex(Geo::Vertex * new_vertex)
{
	bool ret = false;

	if (root->IsIn(new_vertex))
		ret = root->AddVertex(new_vertex);
	else
		ret = ReCalculate(new_vertex);

	return ret;
}

bool KDTreeVertex::AddVertices(const GLfloat * const new_vertices, int num_vertices, const GLuint * const new_indices, int num_indices)
{
	std::vector<Geo::Vertex*> all_vertices;

	AABB limits;
	limits.SetNegativeInfinity();

	int i = 0;

	for (; i < num_vertices; i++)
	{
		Geo::Vertex* vertex = new Geo::Vertex(&new_vertices[i * 3]);
		all_vertices.push_back(vertex);
		limits.Enclose(vertex->MGLVec());
	}

	int num_triangles = num_indices / 3;

	for (i = 0; i < num_triangles; i++)
	{
		Geo::Vertex* v1 = all_vertices[new_indices[i * 3]];
		Geo::Vertex* v2 = all_vertices[new_indices[i * 3 + 1]];
		Geo::Vertex* v3 = all_vertices[new_indices[i * 3 + 2]];

		Geo::Triangle* triangle = new Geo::Triangle(*v1, *v2, *v3);

		v1->AddTriangle(triangle);
		v2->AddTriangle(triangle);
		v3->AddTriangle(triangle);
	}

	root = new KDTNodeVertex(limits);

	if (all_vertices.size() > 0)
	{
		for (std::vector<Geo::Vertex*>::const_iterator it = all_vertices.begin(); it != all_vertices.end(); ++it)
			if (root->AddVertex(*it) == false)
			{
				LOG("Vertex: '%f, %f, %f' could not be added to KDT", (*it)->x, (*it)->y, (*it)->z);
				return false;
			}
	}

	return true;
}

void KDTreeVertex::Draw() const
{
	root->Draw();
}
