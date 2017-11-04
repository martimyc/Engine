#ifndef KDTREE
#define KDTREE

#include <vector>
#include "MathGeoLib\src\MathGeoLibFwd.h"

#define MAX_NUM_OBJECTS 3

class GameObject;

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
	const GameObject* game_objects[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNode* childs[2];
	float median;

public:
	KDTNode(const math::vec min_point, const math::vec max_point);
	KDTNode(const AABB& limits);
	~KDTNode();

	void SubDivide3D(const GameObject* new_game_object);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedian(PARTITION_AXIS partition_axis, const GameObject* new_game_object) const;

	bool AddGameObject(const GameObject* new_game_object);

	bool AddToCorrectChild(const GameObject* new_game_object);

	bool RemoveGameObject(const GameObject* new_game_object);

	void ReArrange();

	bool Empty() const;

	void GetGameObjects(std::vector<const GameObject*>& vec) const;

	bool IsIn(const GameObject* new_game_object) const;

	void Draw() const;
};

class KDTree
{
private:
	KDTNode* root;
	AABB limits;

	bool ReCalculate(GameObject* new_game_object);
public:
	KDTree();
	~KDTree();

	bool AddGameObject(GameObject* new_game_object);

	void Draw() const;
};

#endif // !KDTREE


