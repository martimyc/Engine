#include "KDTree.h"

KDTNode::KDTNode(const AABB& limits): partition_axis(NO_PARTITION), limits(limits)
{
	for(int i = 0; i < MAX_NUM_OBJECTS; i++)
		game_objects[i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;

	for (int i = 0; i < 6; i++)
		overlap[i] = 0.0f;
}

KDTNode::~KDTNode()
{
	if (childs[0] != nullptr)
		delete[] childs;

	//Dont think deleting gameobjects through tree is best
}

void KDTNode::CalculateTree(std::vector<GameObject*> game_objects_vec)
{
	unsigned int num_game_objects = game_objects_vec.size();

	if (num_game_objects < MAX_NUM_OBJECTS)
	{
		memcpy(game_objects, &game_objects_vec, num_game_objects * sizeof(GameObject*));
		return;
	}

	FindBestMedian()
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
