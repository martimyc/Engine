#ifndef KDTREE_GO
#define KDTREE_GO

#include <vector>
#include "MathGeoLib\src\MathGeoLibFwd.h"

#define MAX_NUM_OBJECTS 4

class GameObject;

enum PARTITION_AXIS
{
	X = 0,
	Y = 1,
	Z = 2,
	NO_PARTITION
};

class KDTNodeGO
{
private:
	AABB* limits;
	const GameObject* game_objects[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeGO* childs[2];
	float median;

public:
	KDTNodeGO(const math::vec min_point, const math::vec max_point);
	KDTNodeGO(const AABB& limits);
	~KDTNodeGO();

	bool SubDivide3D(const GameObject* new_game_object);

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

	bool AllSamePos(const GameObject* new_game_object) const;
};

class KDTreeGO
{
private:
	KDTNodeGO* root;
	AABB limits;

	bool ReCalculate(GameObject* new_game_object);
public:
	KDTreeGO();
	~KDTreeGO();

	bool AddGameObject(GameObject* new_game_object);

	void Draw() const;
};

#endif // !KDTREE_GO


