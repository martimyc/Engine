#ifndef KDTREE_GO
#define KDTREE_GO

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"

#define MAX_NUM_OBJECTS 9
#define MAX_SUBDIVISIONS 5

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
	AABB limits;
	const GameObject* game_objects[MAX_NUM_OBJECTS];

	//Partition
	PARTITION_AXIS partition_axis;
	KDTNodeGO* childs[2];
	float median;

public:
	KDTNodeGO(const math::vec min_point, const math::vec max_point);
	KDTNodeGO(const AABB& limits);
	~KDTNodeGO();

private:
	bool SubDivide3D(const GameObject* new_game_object, unsigned int& num_subdivisions);

	void SubDivideChilds(PARTITION_AXIS partition_axis, float median);

	void SubDivide(PARTITION_AXIS partition_axis, float median);

	float FindBestMedianX(const GameObject* new_game_object) const;

	float FindBestMedianY(const GameObject* new_game_object) const;

	float FindBestMedianZ(const GameObject* new_game_object) const;

	bool AllSamePos(const GameObject* new_game_object) const;

	void ReArrange();

	bool AddToCorrectChild(const GameObject* new_game_object, unsigned int& num_subdivisions);

	bool Empty() const;

	bool AllreadySaved(const GameObject* new_game_object) const;

public:
	bool AddGameObject(const GameObject* new_game_object, unsigned int& num_subdivisions);

	bool RemoveGameObject(const GameObject* new_game_object);

	void GetGameObjects(std::vector<const GameObject*>& vec) const;

	void Draw() const;

	bool UpdateGO(const GameObject* updated_go);

	void DeleteHirarchy();

	bool AllIn(const GameObject* new_game_object) const;
	bool IsIn(const GameObject* new_game_object) const;
};

class KDTreeGO
{
private:
	KDTNodeGO* root;

	bool ReCalculate(const GameObject* new_game_object);
public:
	KDTreeGO();
	~KDTreeGO();

	bool AddGameObject(const GameObject* new_game_object);

	bool RemoveGameObject(const GameObject* new_game_object);

	bool UpdateGO(const GameObject* updated_go);

	void Draw() const;
};

#endif // !KDTREE_GO