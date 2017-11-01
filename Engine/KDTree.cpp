#include "KDTree.h"

KDTNode::KDTNode(): partition_axis(NO_PARTITION)
{
	for(int i = 0; i < MAX_NUM_OBJECTS; i++)
		objects [i] = nullptr;

	childs[0] = nullptr;
	childs[1] = nullptr;
}

KDTNode::~KDTNode()
{}

KDTree::KDTree()
{}

KDTree::~KDTree()
{}
