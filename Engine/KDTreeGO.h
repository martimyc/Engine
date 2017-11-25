#ifndef KDTREE_GO
#define KDTREE_GO

#include <vector>
#include "MathGeoLib\src\Geometry\AABB.h"

#define MAX_NUM_OBJECTS 3
#define MAX_SUBDIVISIONS 5 //allways odd, never even

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

	float FindBestMedian(PARTITION_AXIS partition_axis, const GameObject* new_game_object) const;

	float FindBestMedianX(const GameObject* new_game_object) const;

	float FindBestMedianY(const GameObject* new_game_object) const;

	float FindBestMedianZ(const GameObject* new_game_object) const;

public:
	bool AddGameObject(const GameObject* new_game_object, unsigned int& num_subdivisions);

	bool AddToCorrectChild(const GameObject* new_game_object, unsigned int& num_subdivisions);

	bool RemoveGameObject(const GameObject* new_game_object);

	void ReArrange();

	bool Empty() const;

	void GetGameObjects(std::vector<const GameObject*>& vec) const;

	bool IsIn(const GameObject* new_game_object) const;
	bool AllIn(const GameObject* new_game_object) const;	

	void Draw() const;

	bool AllSamePos(const GameObject* new_game_object) const;

	bool UpdateGO(const GameObject* updated_go);

	void DeleteHirarchy();
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

	bool UpdateGO(const GameObject* updated_go);

	void Draw() const;
};

//Subdivide priority queue operators
struct CompareMaxPositionsX
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

struct CompareMaxPositionsY
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

struct CompareMaxPositionsZ
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

struct CompareMinPositionsX
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

struct CompareMinPositionsY
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

struct CompareMinPositionsZ
{
	bool operator()(const GameObject * go1, const GameObject * go2);
};

#endif // !KDTREE_GO


