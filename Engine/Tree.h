#ifndef TREE
#define TREE

#include <vector>

class GameObject;

class TreeNode
{
private:
	std::vector<TreeNode*> childs;
	unsigned int num_childs = 0;
	TreeNode* const parent;
	GameObject* const data;

public:
	TreeNode(GameObject* const data, TreeNode* const parent);
	~TreeNode();

	bool AddChild(GameObject* new_child);	

	bool AddChild(TreeNode* new_child);	

	bool Remove(const TreeNode* const remove);	

	void Draw() const;

	TreeNode* const GetPatent() const;	

	GameObject* const GetData() const;	
};

class Tree
{
private:
	TreeNode* root = new TreeNode(nullptr, nullptr);
	unsigned int num_nodes = 0;
	TreeNode* focused = root;

public:
	Tree();
	~Tree();

	GameObject* const CreateGameobject(); //this funct will create the game objects

	void Focus(TreeNode* new_focus);

	void Move(TreeNode* move);

	GameObject* GetFocused() const;

	void Empty();

	void Draw() const;
};

#endif //!TREE

