#ifndef KDTREE
#define KDTREE

#include "GameObject.h"
#include "MathGeoLib\src\MathGeoLib.h"

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
	PARTITION_AXIS partition_axis;
	KDTNode* childs[2];
	GameObject* objects [MAX_NUM_OBJECTS];

public:
	KDTNode();
	~KDTNode();

};

class KDTree
{
private:

public:
	KDTree();
	~KDTree();


};

#endif // !KDTREE


