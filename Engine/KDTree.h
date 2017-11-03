#ifndef KDTREE
#define KDTREE

#include <vector>
#include "MathGeoLib\src\MathGeoLib.h"
#include "GameObject.h"

#define MAX_NUM_OBJECTS 3

enum PARTITION_AXIS
{
	X = 0,
	Y = 1,
	Z = 2,
	NO_PARTITION
};

class KDTNode
{
private:
	AABB* limits;
	GameObject* game_objects[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNode* childs[2];
	float median;

public:
	KDTNode(const math::vec min_point, const math::vec max_point);
	~KDTNode();

	void SubDivide3D();

	void SubDivideChilds(PARTITION_AXIS partition_axis);

	void SubDivide(PARTITION_AXIS partition_axis);

	float FindBestMedian(PARTITION_AXIS partition_axis) const;

	bool AddGameObject(const GameObject* new_game_object);

	bool AddToCorrectChild(const GameObject* new_game_object);

	bool RemoveGameObject(const GameObject* new_game_object);

	void ReArrange();

	bool Empty() const;
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


