#ifndef TREE
#define TREE

#include <vector>

template <class DATA>
class TreeNode
{
private:
	std::vector<TreeNode<DATA>*> childs;
	unsigned int num_childs = 0;
	TreeNode<DATA>* const parent;
	const DATA* const data;

public:
	TreeNode(const DATA* const data; const TreeNode<DATA>* const parent) : data(data), parent(parent)
	{}
	~TreeNode()
	{}

	void AddChild(DATA* new_child)
	{
		childs.push_back(new TreeNode(new_child));
	}

	Remove(const TreeNode<DATA>* const remove)
	{
		for (std::vector<TreeNode<DATA>*>::const_iterator it = childs.begin(); it = childs.end(); ++it)
			if ((*it) == remove)
				childs.erase(it);
	}
};

template <class DATA>
class Tree
{
private:
	const TreeNode<DATA>* const root = new TreeNode();
	unsigned int num_nodes = 0;
	TreeNode<DATA>* focused = nullptr;

public:
	void AddGameObject() //this funct will create the <DATA> objects
	{
		if (focused != nullptr)
			focused->AddChild(new DATA(focused));
	}

	Focus(TreeNode<DATA>* new_focus)
	{
		focused = new_focus;
	}

	Move(TreeNode<DATA>* move)
	{
		move->parent->Remove(move);
		focused->AddChild(move)
	}
};

#endif //!TREE

