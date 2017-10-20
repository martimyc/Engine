#ifndef TREE
#define TREE

#include <vector>
#include <string>

class GameObject;

class TreeNode
{
private:
	std::string name;
	std::vector<TreeNode*> childs;
	unsigned int num_childs = 0;
	TreeNode* const parent;
	GameObject* const data;

public:
	TreeNode(GameObject* const data, TreeNode* const parent, const char* name = nullptr);
	~TreeNode();

	TreeNode* AddChild(GameObject* new_child);

	bool AddChild(TreeNode* new_child);	

	bool Remove(const TreeNode* const remove);	

	void Draw() const;

	TreeNode* const GetPatent() const;	

	GameObject* const GetData() const;

	void Hirarchy(int & num_nodes, int& selection_mask);
};

class Tree
{
private:
	TreeNode* root = new TreeNode(nullptr, nullptr);
	unsigned int num_nodes = 0;
	unsigned int num_game_objects = 0;
	unsigned int num_groups = 0;
	TreeNode* focused = root;
	bool hirarchy_active = true;

public:
	Tree();
	~Tree();

	GameObject* const CreateGameobject(const char* const name = nullptr); //this funct will create the game objects
	TreeNode* const CreateGroup(const char* const name = nullptr);

	void Focus(TreeNode* new_focus);

	void Move(TreeNode* move);

	GameObject* GetFocused() const;

	void Empty();

	void Draw() const;

	void Hirarchy();

	void FocusRoot();

	TreeNode* GetRoot() const;
};

#endif //!TREE

