#include <functional>
#include <queue>
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "GameObject.h"
#include "KDTreeGO.h"

KDTNodeGO::KDTNodeGO(const math::vec min_point, const math::vec max_point): partition_axis(NO_PARTITION), limits(min_point, max_point)
{
	for(int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeGO::KDTNodeGO(const AABB& limits) : partition_axis(NO_PARTITION), limits(limits)
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNodeGO::~KDTNodeGO() //Dont think deleting gameobjects through tree is best
{
	if (childs[0] != nullptr)
	{
		delete childs[0];
		delete childs[1];
	}
}

bool KDTNodeGO::SubDivide3D(const GameObject* new_game_object, unsigned int& num_subdivisions)
{
	if (++num_subdivisions >= MAX_SUBDIVISIONS)
	{
		LOG("Subdividing over limit");
		return false;
	}

	if (!AllSamePos(new_game_object))
	{
		float median_x = FindBestMedianX(new_game_object);
		float median_y = FindBestMedianY(new_game_object);
		float median_z = FindBestMedianZ(new_game_object);

		SubDivide(X, median_x);
		SubDivideChilds(Y, median_y);
		childs[0]->SubDivideChilds(Z, median_z);
		childs[1]->SubDivideChilds(Z, median_z);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			if (AddToCorrectChild(game_objects[i], num_subdivisions) == false)
				return false;

			game_objects[i] = nullptr;
		}

		if (AddToCorrectChild(new_game_object, num_subdivisions) == false)
			return false;

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

		childs[0] = new KDTNodeGO(min_point, max_point);

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

		childs[1] = new KDTNodeGO(min_point, max_point);
	}
	else
		LOG("Trying to subdivide node with existing childs");
}

float KDTNodeGO::FindBestMedianX(const GameObject * new_game_object) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> max_queue;
	std::priority_queue<float> min_queue;

	bool repeated = false;

	max_queue.push(new_game_object->GetMaxPos().x);
	min_queue.push(new_game_object->GetMinPos().x);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().x == new_game_object->GetMaxPos().x)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().x == game_objects[j]->GetMaxPos().x)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().x);

		repeated = false;
		if (game_objects[i]->GetMinPos().x == new_game_object->GetMinPos().x)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMinPos().x == game_objects[j]->GetMinPos().x)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			min_queue.push(game_objects[i]->GetMinPos().x);
	}

	float current_max(max_queue.top());
	float current_min(min_queue.top());
	float median = inf;

	while (max_queue.size() != 0 && min_queue.size() != 0)
	{
		if (current_max < current_min)
			median = current_max + abs(current_min - current_max) / 2.0f;
		else
			break;

		if (max_queue.size() == 1 || min_queue.size() == 1)
			break;

		min_queue.pop();
		if (current_max < min_queue.top())
			median = current_max + abs(min_queue.top() - current_max) / 2.0f;

		max_queue.pop();
		if (current_min > max_queue.top())
			median = max_queue.top() + abs(current_min - max_queue.top()) / 2.0f;;

		current_max = max_queue.top();
		current_min = min_queue.top();
	}

	if (median != inf)
		return median;

	LOG("Median X could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free
	while (max_queue.size() != 0)
		max_queue.pop();
	max_queue.push(new_game_object->GetMaxPos().x);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().x == new_game_object->GetMaxPos().x)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().x == game_objects[j]->GetMaxPos().x)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().x);
	}

	if (max_queue.size() > 1)
	{
		for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
			max_queue.pop();

		float halve_max_big = max_queue.top();
		max_queue.pop();
		float halve_max_small = max_queue.top();

		return ((halve_max_big + halve_max_small) / 2.0f);
	}
	else
	{
		while (min_queue.size() != 0)
			min_queue.pop();
		min_queue.push(new_game_object->GetMinPos().y);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			repeated = false;
			if (game_objects[i]->GetMinPos().x == new_game_object->GetMinPos().x)
				repeated = true;

			if (i > 0)
			{
				for (int j = i - 1; j >= 0; j--)
					if (game_objects[i]->GetMinPos().x == game_objects[j]->GetMinPos().x)
					{
						repeated = true;
						break;
					}
			}

			if (repeated == false)
				min_queue.push(game_objects[i]->GetMinPos().y);
		}

		if (min_queue.size() > 1)
		{
			for (int i = min_queue.size() / 2 - 1; i > 0; i--) //Empty biggest min & smallest min
				min_queue.pop();

			float halve_min_big = min_queue.top();
			min_queue.pop();
			float halve_min_small = min_queue.top();

			return ((halve_min_big + halve_min_small) / 2.0f);
		}
	}
	return limits.CenterPoint().x;
}

float KDTNodeGO::FindBestMedianY(const GameObject * new_game_object) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> max_queue;
	std::priority_queue<float> min_queue;

	bool repeated = false;

	max_queue.push(new_game_object->GetMaxPos().y);
	min_queue.push(new_game_object->GetMinPos().y);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().y == new_game_object->GetMaxPos().y)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().y == game_objects[j]->GetMaxPos().y)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().y);

		repeated = false;
		if (game_objects[i]->GetMinPos().y == new_game_object->GetMinPos().y)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMinPos().y == game_objects[j]->GetMinPos().y)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			min_queue.push(game_objects[i]->GetMinPos().y);
	}

	float current_max(max_queue.top());
	float current_min(min_queue.top());
	float median = inf;

	while (max_queue.size() != 0 && min_queue.size() != 0)
	{
		if (current_max < current_min)
			median = current_max + abs(current_min - current_max) / 2.0f;
		else
			break;

		if (max_queue.size() == 1 || min_queue.size() == 1)
			break;

		min_queue.pop();
		if (current_max < min_queue.top())
			median = current_max + abs(min_queue.top() - current_max) / 2.0f;

		max_queue.pop();
		if (current_min > max_queue.top())
			median = max_queue.top() + abs(current_min - max_queue.top()) / 2.0f;;

		current_max = max_queue.top();
		current_min = min_queue.top();
	}

	if (median != inf)
		return median;

	LOG("Median Y could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free
	while (max_queue.size() != 0)
		max_queue.pop();
	max_queue.push(new_game_object->GetMaxPos().y);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().y == new_game_object->GetMaxPos().y)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().y == game_objects[j]->GetMaxPos().y)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().y);
	}

	if (max_queue.size() > 1)
	{
		for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
			max_queue.pop();

		float halve_max_big = max_queue.top();
		max_queue.pop();
		float halve_max_small = max_queue.top();

		return ((halve_max_big + halve_max_small) / 2.0f);
	}
	else
	{
		while (min_queue.size() != 0)
			min_queue.pop();
		min_queue.push(new_game_object->GetMinPos().y);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			repeated = false;
			if (game_objects[i]->GetMinPos().y == new_game_object->GetMinPos().y)
				repeated = true;

			if (i > 0)
			{
				for (int j = i - 1; j >= 0; j--)
					if (game_objects[i]->GetMinPos().y == game_objects[j]->GetMinPos().y)
					{
						repeated = true;
						break;
					}
			}

			if (repeated == false)
				min_queue.push(game_objects[i]->GetMinPos().y);
		}

		if (min_queue.size() > 1)
		{
			for (int i = min_queue.size() / 2 - 1; i > 0; i--) //Empty biggest min & smallest min
				min_queue.pop();

			float halve_min_big = min_queue.top();
			min_queue.pop();
			float halve_min_small = min_queue.top();

			return ((halve_min_big + halve_min_small) / 2.0f);
		}
	}
	return limits.CenterPoint().y;
}

float KDTNodeGO::FindBestMedianZ(const GameObject * new_game_object) const
{
	std::priority_queue<float, std::vector<float>, std::greater<float>> max_queue;
	std::priority_queue<float> min_queue;

	bool repeated = false;

	max_queue.push(new_game_object->GetMaxPos().z);
	min_queue.push(new_game_object->GetMinPos().z);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().z == new_game_object->GetMaxPos().z)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().z == game_objects[j]->GetMaxPos().z)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().z);

		repeated = false;
		if (game_objects[i]->GetMinPos().z == new_game_object->GetMinPos().z)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMinPos().z == game_objects[j]->GetMinPos().z)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			min_queue.push(game_objects[i]->GetMinPos().z);
	}

	float current_max(max_queue.top());
	float current_min(min_queue.top());
	float median = inf;

	while (max_queue.size() != 0 && min_queue.size() != 0)
	{
		if (current_max < current_min)
			median = current_max + abs(current_min - current_max) / 2.0f;
		else
			break;

		if (max_queue.size() == 1 || min_queue.size() == 1)
			break;

		min_queue.pop();
		if (current_max < min_queue.top())
			median = current_max + abs(min_queue.top() - current_max) / 2.0f;

		max_queue.pop();
		if (current_min > max_queue.top())
			median = max_queue.top() + abs(current_min - max_queue.top()) / 2.0f;;

		current_max = max_queue.top();
		current_min = min_queue.top();
	}

	if (median != inf)
		return median;

	LOG("Median Z could not be found");

	//if median can't be found we return point in between max points so that at least one of the nodes will be halve free
	while (max_queue.size() != 0)
		max_queue.pop();
	max_queue.push(new_game_object->GetMaxPos().z);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		repeated = false;
		if (game_objects[i]->GetMaxPos().z == new_game_object->GetMaxPos().z)
			repeated = true;

		if (i > 0)
		{
			for (int j = i - 1; j >= 0; j--)
				if (game_objects[i]->GetMaxPos().z == game_objects[j]->GetMaxPos().z)
				{
					repeated = true;
					break;
				}
		}

		if (repeated == false)
			max_queue.push(game_objects[i]->GetMaxPos().z);
	}

	if (max_queue.size() > 1)
	{
		for (int i = max_queue.size() / 2 - 1; i > 0; i--) //Empty biggest max & smallest min
			max_queue.pop();

		float halve_max_big = max_queue.top();
		max_queue.pop();
		float halve_max_small = max_queue.top();

		return ((halve_max_big + halve_max_small) / 2.0f);
	}
	else
	{
		while (min_queue.size() != 0)
			min_queue.pop();
		min_queue.push(new_game_object->GetMinPos().y);

		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
		{
			repeated = false;
			if (game_objects[i]->GetMinPos().z == new_game_object->GetMinPos().z)
				repeated = true;

			if (i > 0)
			{
				for (int j = i - 1; j >= 0; j--)
					if (game_objects[i]->GetMinPos().z == game_objects[j]->GetMinPos().z)
					{
						repeated = true;
						break;
					}
			}

			if (repeated == false)
				min_queue.push(game_objects[i]->GetMinPos().y);
		}

		if (min_queue.size() > 1)
		{
			for (int i = min_queue.size() / 2 - 1; i > 0; i--) //Empty biggest min & smallest min
				min_queue.pop();

			float halve_min_big = min_queue.top();
			min_queue.pop();
			float halve_min_small = min_queue.top();

			return ((halve_min_big + halve_min_small) / 2.0f);
		}
	}
	return limits.CenterPoint().z;
}

bool KDTNodeGO::AddGameObject(const GameObject * new_game_object, unsigned int& num_subdivisions)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		if (AllreadySaved(new_game_object))
			LOG("Adding repeated go");
		else
		{
			if (game_objects[MAX_NUM_OBJECTS - 1] == nullptr)
			{
				for (int i = 0; i < MAX_NUM_OBJECTS; i++)
					if (game_objects[i] == nullptr)
					{
						game_objects[i] = new_game_object;
						ret = true;
						break;
					}
			}
			else
			{
				if (SubDivide3D(new_game_object, num_subdivisions) == false)
					return false;
				else
					ret = true;
			}
		}
	}
	else
	{
		if (AddToCorrectChild(new_game_object, num_subdivisions) == false)
			return false;
		else
			ret = true;
	}

	return ret;
}

bool KDTNodeGO::AddToCorrectChild(const GameObject * new_game_object, unsigned int& num_subdivisions)
{
	bool ret = false;

	if (new_game_object->HasMeshFilter())
	{
		math::vec max_pos(new_game_object->GetMaxPos());
		math::vec min_pos(new_game_object->GetMinPos());

		if (min_pos[partition_axis] <= median)
			if (childs[0]->AddGameObject(new_game_object, num_subdivisions) == false)
				return false;
			else
				ret = true;

		if (max_pos[partition_axis] >= median)
			if (childs[1]->AddGameObject(new_game_object, num_subdivisions) == false)
				return false;
			else
				ret = true;
	}
	else
	{
		if (new_game_object->GetWorldPosition()[partition_axis] <= median)
			if (childs[0]->AddGameObject(new_game_object, num_subdivisions) == false)
				return false;
			else
				ret = true;

		if (new_game_object->GetWorldPosition()[partition_axis] > median)
			if (childs[1]->AddGameObject(new_game_object, num_subdivisions) == false)
				return false;
			else
				ret = true;
	}


	if (ret == false)
		LOG("Gameobject does not intersect with any child");

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
	{
		if (childs[0]->IsIn(new_game_object))
			if (childs[0]->RemoveGameObject(new_game_object))
				ret = true;
		if (childs[1]->IsIn(new_game_object))
			if (childs[1]->RemoveGameObject(new_game_object))
				ret = true;
	}

	if (partition_axis == Z && Empty())
		if (childs != nullptr)
		{
			childs[0]->DeleteHirarchy();
			childs[1]->DeleteHirarchy();
		}

	return ret;
}

void KDTNodeGO::ReArrange()
{
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (game_objects[i] == nullptr && game_objects[i + 1] != nullptr)
		{
			game_objects[i] = game_objects[i + 1];
			game_objects[i + 1] = nullptr;
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
		{
			for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			{
				if (game_objects[i] != nullptr)
				{
					bool repeated = false;
					for (std::vector<const GameObject*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
						if ((*it) == game_objects[i])
							repeated = true;
					if (!repeated)
						vec.push_back(game_objects[i]);
				}
				else
					break;
			}
		}
	}
}

bool KDTNodeGO::IsIn(const GameObject * new_game_object) const
{
	math::vec max_pos(new_game_object->GetMaxPos());
	math::vec min_pos(new_game_object->GetMinPos());

	if (max_pos.x < limits.minPoint.x || min_pos.x > limits.maxPoint.x)
		return false;
	if (max_pos.y < limits.minPoint.y || min_pos.y > limits.maxPoint.y)
		return false;
	if (max_pos.z < limits.minPoint.z || min_pos.z > limits.maxPoint.z)
		return false;

	return true;
}

bool KDTNodeGO::AllIn(const GameObject * new_game_object) const
{
	math::vec max_point(new_game_object->GetMaxPos());
	math::vec min_point(new_game_object->GetMinPos());

	if (limits.Contains(max_point) && limits.Contains(min_point)) //TODO sphere and oobb checks when those are done
		return true;
	return false;
}

bool KDTNodeGO::AllreadySaved(const GameObject * new_game_object) const
{
	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		if (game_objects[i] != nullptr)
		{
			if (game_objects[i] == new_game_object)
				return true;
		}
		else
			break;			
	}
	return false;
}

void KDTNodeGO::Draw() const
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
		if (position.x != game_objects[i]->GetWorldPosition().x || position.y != game_objects[i]->GetWorldPosition().y || position.z != game_objects[i]->GetWorldPosition().z)
			return false;
	return true;
}

bool KDTNodeGO::UpdateGO(const GameObject * updated_go)
{
	bool ret = false;

	if (partition_axis != NO_PARTITION)
	{

		if (childs[0]->AllIn(updated_go) == false || childs[1]->AllIn(updated_go) == false)
		{
			RemoveGameObject(updated_go);
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

void KDTNodeGO::DeleteHirarchy()
{
	median = 0; 
	partition_axis = NO_PARTITION;
	DELETE_PTR(childs[0]);
	DELETE_PTR(childs[1]);
}

KDTreeGO::KDTreeGO()
{
	AABB limits;
	limits.SetNegativeInfinity();
	root = new KDTNodeGO(limits);
}

KDTreeGO::~KDTreeGO()
{}

bool KDTreeGO::ReCalculate(const GameObject* new_game_object)
{
	std::vector<const GameObject*> all_game_objects;

	root->GetGameObjects(all_game_objects);

	delete root;

	AABB limits;
	limits.SetNegativeInfinity();

	limits.Enclose(new_game_object->GetMaxPos());
	if(new_game_object->HasMeshFilter())
		limits.Enclose(new_game_object->GetMinPos());

	if (all_game_objects.size() > 0)
	{
		for (std::vector<const GameObject*>::const_iterator it = all_game_objects.begin(); it != all_game_objects.end(); ++it)
		{
			limits.Enclose((*it)->GetMaxPos());
			if ((*it)->HasMeshFilter())
				limits.Enclose((*it)->GetMinPos());
		}
	}

	root = new KDTNodeGO(limits);

	unsigned int num_subdivisions = 0;
	if (root->AddGameObject(new_game_object, num_subdivisions) == false)
	{
		LOG("New GameObject: '%s' could not be added to KDT after recalculating", new_game_object->GetName().c_str());
		return false;
	}

	if (all_game_objects.size() > 0)
	{
		for (std::vector<const GameObject*>::const_iterator it = all_game_objects.begin(); it != all_game_objects.end(); ++it)
		{
			num_subdivisions = 0;
			if (root->AddGameObject(*it, num_subdivisions) == false)
			{
				LOG("GameObject: '%s' could not be re-added to KDT after recalculating", (*it)->GetName().c_str());
				return false;
			}
		}
	}

	return true;
}

bool KDTreeGO::AddGameObject(const GameObject * new_game_object)
{
	if (root->AllIn(new_game_object))
	{
		unsigned int num_subdivisions = 0;
		if (root->AddGameObject(new_game_object, num_subdivisions) == false)
			return false;
	}
	else
		if (ReCalculate(new_game_object) == false)
			return false;

	if (new_game_object->AddChildsToKDT(*this) == false)
		return false;

	return true;
}

bool KDTreeGO::RemoveGameObject(const GameObject * new_game_object)
{
	return root->RemoveGameObject(new_game_object);
}

bool KDTreeGO::UpdateGO(const GameObject * updated_go)
{
	if (root->AllIn(updated_go))
		return root->UpdateGO(updated_go);
	else
		if(root->RemoveGameObject(updated_go))
			return ReCalculate(updated_go);
}

void KDTreeGO::Draw() const
{
	root->Draw();
}