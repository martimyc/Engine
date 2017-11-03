#ifndef KDTREE
#define KDTREE

#include <vector>
#include "MathGeoLib\src\MathGeoLib.h"
#include "GameObject.h"

#define MAX_NUM_OBJECTS 3

enum PARTITION_AXIS
{
	NO_PARTITION = 0,
	X,
	Y,
	Z
};

class KDTNode
{
private:
	AABB limits;
	KDTNode* childs[2];
	GameObject* game_objects[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	float median;

public:
	KDTNode(const AABB& limits);
	~KDTNode();

	void CalculateTree(std::vector<GameObject*> game_objects_vec);

	void FindBestMedian(float& median, PARTITION_AXIS& partition_axis);

	void UpdateOverlaps(const GameObject* const new_game_object);

	//Intersects
	//void Intersect(vector<GameObject*>&, PRIMITIVE)
};

class KDTree
{
private:
	KDTNode* root;
	AABB limits;

public:
	KDTree();
	~KDTree();

	void ReCalculate(std::vector<GameObject*> all_game_objects);

};

#endif // !KDTREE


