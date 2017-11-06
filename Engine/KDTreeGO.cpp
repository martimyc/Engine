#include <functional>
#include <queue>
#include <vector>
#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "GameObject.h"
#include "KDTreeGO.h"

KDTNodeGO::KDTNodeGO(const math::vec min_point, const math::vec max_point): partition_axis(NO_PARTITION)
{
	limits = new AABB(min_point, max_point);
	for(int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeGO::KDTNodeGO(const AABB& limits) : partition_axis(NO_PARTITION)
{
	this->limits = new AABB(limits);
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeGO::~KDTNodeGO()
{
	delete limits;

	if (childs[0] != nullptr)
		delete[] childs;

	//Dont think deleting gameobjects through tree is best
}

bool KDTNodeGO::SubDivide3D(const GameObject* new_game_object)
{
	if (!AllSamePos(new_game_object))
	{
		float median_x = FindBestMedian(X, new_game_object);
		float median_y = FindBestMedian(Y, new_game_object);
		float median_z = FindBestMedian(Z, new_game_object);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			math::vec vect(vec::zero);
			vect[0] = 3;
			if (game_objects[i]->GetWorldPosition()[partition_axis] <= median)
				childs[0]->AddGameObject(game_objects[i]);

			if (game_objects[i]->GetWorldPosition()[partition_axis] > median)
				childs[1]->AddGameObject(game_objects[i]);

			game_objects[i] = nullptr;
		}
		return true;
	}
	else
		LOG("Trying to subdivide with all objects in the same place");
	return false;
}

void KDTNodeGO::SubDivideChilds(PARTITION_AXIS partition_axis, float median)
{
	if (childs[0] != nullptr)
	{
		childs[0]->SubDivide(partition_axis, median);
		childs[1]->SubDivide(partition_axis, median);
	}
	else
		LOG("No childs to subdivide");
}

void KDTNodeGO::SubDivide(PARTITION_AXIS partition_axis, float median)
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

		childs[0] = new KDTNodeGO(min_point, max_point);

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

		childs[1] = new KDTNodeGO(min_point, max_point);
	}
	else
		LOG("Trying to subdivide node with existing childs");
}

float KDTNodeGO::FindBestMedian(PARTITION_AXIS partition_axis, const GameObject* new_game_object) const
{
	std::priority_queue <float, std::vector<float>, std::greater<float>> queue;

	bool all_equal = true;

	queue.push(new_game_object->GetWorldPosition()[partition_axis]);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (game_objects[i] != nullptr)
			queue.push(game_objects[i]->GetWorldPosition()[partition_axis]);
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

bool KDTNodeGO::AddGameObject(const GameObject * new_game_object)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (game_objects[i] == nullptr)
			{
				game_objects[i] = new_game_object;
				ret = true;
				break;
			}

		if (ret == false)
		{
			if (SubDivide3D(new_game_object) == false)
				return false;
			if (AddToCorrectChild(new_game_object) == false)
				return false;
		}
	}
	else
		if (AddToCorrectChild(new_game_object) == false)
			return false;

	return ret;
}

bool KDTNodeGO::AddToCorrectChild(const GameObject * new_game_object)
{
	bool ret = false;

	if (new_game_object->GetWorldPosition()[partition_axis] <= median)
		if (childs[0]->AddGameObject(new_game_object) == false)
			return false;
		else
			ret = true;

	if (new_game_object->GetWorldPosition()[partition_axis] > median)
		if (childs[1]->AddGameObject(new_game_object) == false)
			return false;
		else
			ret = true;

	if (ret == false)
		LOG("Gameobject does not fit inside any child");

	return ret;
}

bool KDTNodeGO::RemoveGameObject(const GameObject * new_game_object)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (game_objects[i] == new_game_object)
			{
				game_objects[i] = nullptr;
				ret = true;
				ReArrange();
				break;
			}
	}
	else
		if (childs[0]->RemoveGameObject(new_game_object) || childs[1]->RemoveGameObject(new_game_object))
			ret = true;

	if (partition_axis == Z && Empty())
		delete[] childs;

	return ret;
}

void KDTNodeGO::ReArrange()
{
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (game_objects[i] == nullptr && game_objects[i + 1] != nullptr)
		{
			game_objects[i] = game_objects[i + 1];
			i = 0;
		}
}

bool KDTNodeGO::Empty() const
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

void KDTNodeGO::GetGameObjects(std::vector<const GameObject*>& vec) const
{
	if (partition_axis != NO_PARTITION)
	{
		childs[0]->GetGameObjects(vec);
		childs[1]->GetGameObjects(vec);
	}
	else
	{
		if (game_objects[0] != nullptr)
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (game_objects[i] != nullptr)
					vec.push_back(game_objects[i]);
				else
					break;
			}
	}
}

bool KDTNodeGO::IsIn(const GameObject * new_game_object) const
{
	math::vec position(new_game_object->GetWorldPosition());

	if (position.x < limits->minPoint.x || position.x > limits->maxPoint.x)
		return false;
	if (position.y < limits->minPoint.y || position.y > limits->maxPoint.y)
		return false;
	if (position.z < limits->minPoint.z || position.z > limits->maxPoint.z)
		return false;

	return true;
}

void KDTNodeGO::Draw() const
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
			if (game_objects[i] != nullptr)
			{
				math::vec position = game_objects[i]->GetWorldPosition();
				glVertex3f(position.x, position.y, position.z);
			}
			else
				break;
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

bool KDTNodeGO::AllSamePos(const GameObject * new_game_object) const
{
	math::vec position = new_game_object->GetWorldPosition();

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		if (position.x != game_objects[i]->GetLocalPosition().x || position.y != game_objects[i]->GetLocalPosition().y || position.z != game_objects[i]->GetLocalPosition().z)
			return false;
	return true;
}

KDTreeGO::KDTreeGO()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeGO(limits);
}

KDTreeGO::~KDTreeGO()
{}

bool KDTreeGO::ReCalculate(GameObject* new_game_object)
{
	std::vector<const GameObject*> all_game_objects;

	root->GetGameObjects(all_game_objects);

	delete root;

	AABB limits;
	limits.SetNegativeInfinity();

	limits.Enclose(&new_game_object->GetWorldPosition(), 1);

	if (all_game_objects.size() > 0)
	{
		for (std::vector<const GameObject*>::const_iterator it = all_game_objects.begin(); it != all_game_objects.end(); ++it)
			limits.Enclose(&(*it)->GetWorldPosition(), 1);
	}

	root = new KDTNodeGO(limits);

	if (root->AddGameObject(new_game_object) == false)
	{
		LOG("New GameObject: '%s' could not be added to KDT after recalculating", new_game_object->GetName().c_str());
		return false;
	}

	if (all_game_objects.size() > 0)
	{
		for (std::vector<const GameObject*>::const_iterator it = all_game_objects.begin(); it != all_game_objects.end(); ++it)
			if (root->AddGameObject(*it) == false)
			{
				LOG("GameObject: '%s' could not be re-added to KDT after recalculating", (*it)->GetName().c_str());
				return false;
			}
	}

	return true;
}

bool KDTreeGO::AddGameObject(GameObject * new_game_object)
{
	bool ret = false;

	if (root->IsIn(new_game_object))
		ret = root->AddGameObject(new_game_object);
	else
		ret = ReCalculate(new_game_object);

	return ret;
}

void KDTreeGO::Draw() const
{
	root->Draw();
}
