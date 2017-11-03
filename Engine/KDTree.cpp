#include "Globals.h"
#include "KDTree.h"

KDTNode::KDTNode(const math::vec min_point, const math::vec max_point): partition_axis(NO_PARTITION), limits(limits)
{
	limits = new AABB(min_point, max_point);
	for(int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNode::~KDTNode()
{
	delete limits;

	if (childs[0] != nullptr)
		delete[] childs;

	//Dont think deleting gameobjects through tree is best
}

void KDTNode::SubDivide3D()
{
	SubDivide(X);
	SubDivideChilds(Y);
	childs[0]->SubDivideChilds(Z);
	childs[1]->SubDivideChilds(Z);

	for (int i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		if (game_objects[i]->GetWorldPosition()[partition_axis] <= median)
			child[0]->AddGameObject(game_objects[i]);

		if (game_objects[i]->GetWorldPosition()[partition_axis] > median)
			child[1]->AddGameObject(game_objects[i]);

		game_objects[i] = nullptr;
	}
}

void KDTNode::SubDivideChilds(PARTITION_AXIS partition_axis)
{
	if (childs[0] != nullptr)
	{
		childs[0]->SubDivide(partition_axis);
		childs[1]->SubDivide(partition_axis);
	}
	else
		LOG("No childs to subdivide");
}

void KDTNode::SubDivide(PARTITION_AXIS partition_axis)
{
	if (child[0] == nullptr)
	{
		float median = FindBestMedian(partition_axis);
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

		childs[0] = new KDTNode(min_point, max_point);

		max_point = limits->maxPoint;

		switch (partition_axis)
		{
		case X:
			min_point.x = median;
			min_point.y = limits->min_point.y;
			min_point.z = limits->min_point.z;
			break;
		case Y:
			min_point.x = limits->min_point.x;
			min_point.y = median;
			min_point.z = limits->min_point.z;
			break;
		case Z:
			min_point.x = limits->min_point.x;
			min_point.y = limits->min_point.y;
			min_point.z = median;
			break;
		default:
			LOG("Non standard partition axis to subdivide");
			break;
		}

		childs[1] = new KDTNode(min_point, max_point);
	}
	else
		LOG("Trying to subdivide node with existing childs");
}

float KDTNode::FindBestMedian(PARTITION_AXIS partition_axis) const
{
	return 0.0f;
}

bool KDTNode::AddGameObject(const GameObject * new_game_object)
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
			SubDivide3D();
			AddToCorrectChild(new_game_object);
		}
	}
	else
		AddToCorrectChild(new_game_object);

	return ret;
}

bool KDTNode::AddToCorrectChild(const GameObject * new_game_object)
{
	bool ret = false;

	if (new_game_object->GetWorldPosition()[partition_axis] <= median)
		ret = child[0]->AddGameObject(new_game_object);

	if (new_game_object->GetWorldPosition()[partition_axis] > median)
		ret = child[1]->AddGameObject(new_game_object);

	return ret;
}

bool KDTNode::RemoveGameObject(const GameObject * new_game_object)
{
	bool ret = false;

	if (partition_axis == NO_PARTITION)
	{
		for (int i = 0; i < MAX_NUM_OBJECTS; i++)
			if (game_objects[i] == new_game_object)
			{
				game_objects[i] = nullptr;
				ret = true;
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

void KDTNode::ReArrange()
{
	for (int i = 0; i < MAX_NUM_OBJECTS - 1; i++)
		if (game_objects[i] == nullptr && game_objects[i + 1] != nullptr)
		{
			game_objects[i] = game_objects[i + 1];
			i = 0;
		}
}

bool KDTNode::Empty() const
{
	if (partition_axis == NO_PARTITION)
	{
		if (chils[0] == nullptr)
		{
			return true;
		else
			return false;
		}
	}
	else
	{
		if (childs[0]->Empty() && childs[1]->Empty())
			return true;
		else
			return false;
	}
}

KDTree::KDTree()
{
	root = new KDTNode(limits);
}

KDTree::~KDTree()
{}

void KDTree::ReCalculate(std::vector<GameObject*> all_game_objects)
{
	root->CalculateTree(all_game_objects);
}
