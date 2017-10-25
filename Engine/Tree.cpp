#include "imgui\imgui.h"
#include "Globals.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"
#include "Transformation.h"
#include "Tree.h"

TreeNode::TreeNode(GameObject * const data, TreeNode * const parent, const char* name) : data(data), parent(parent)
{
	if (name != nullptr)
		this->name = name;
	else if (data != nullptr)
		this->name = data->GetName();
	else
		this->name = "No name";
}

TreeNode::~TreeNode()
{
	for (std::vector<TreeNode*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete (*it);
	childs.clear();
	delete data;
}

TreeNode* TreeNode::AddChild(GameObject * new_child)
{
	TreeNode* new_node = nullptr;
	
	if (new_child != nullptr)
	{
		new_node = new TreeNode(new_child, this);
		childs.push_back(new_node);
	}
	else
		LOG("Trying to add a null game object");
	return new_node;
}

bool TreeNode::AddChild(TreeNode * new_child)
{
	if (new_child != nullptr)
	{
		childs.push_back(new_child);
		return true;
	}
	return false;
}

bool TreeNode::Remove(const TreeNode * const remove)
{
	for (std::vector<TreeNode*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		if ((*it) == remove)
		{
			childs.erase(it);
			return true;
		}
	return false;
}

void TreeNode::Draw() const
{
	for (std::vector<TreeNode*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		(*it)->Draw();
	if (data != nullptr)
		data->Draw();
}

TreeNode * const TreeNode::GetPatent() const
{
	return parent;
}

GameObject * const TreeNode::GetData() const
{
	return data;
}

bool TreeNode::Hirarchy(TreeNode*& selected_node)
{
	bool ret = false;
	bool has_childs = childs.size() != 0;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected_node == this) ? ImGuiTreeNodeFlags_Selected : 0);
	
	if (!has_childs)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
		node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool node_open = ImGui::TreeNodeEx(this, node_flags, name.c_str()); //TODO try with TreeNode*

	if (ImGui::IsItemClicked() && ret == false)
	{
		selected_node = this;
		ret = true;
	}

	if(node_open)
	{
		for (std::vector<TreeNode*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			if ((*it)->Hirarchy(selected_node))
				ret = true;

		if(has_childs)
			ImGui::TreePop();
	}

	if (selected_node == this && data != nullptr)
		data->Inspector();

	return ret;
}

std::vector<TreeNode*>& TreeNode::GetChildsVec()
{
	return childs;
}

//save this for materials and textures (things that wont expand tree further)
/*
else
{
// Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Selectable Leaf %d", i);
if (ImGui::IsItemClicked())
node_clicked = i;
}*/

Tree::Tree(): root(new TreeNode(nullptr, nullptr, "Root")), num_nodes(0), num_game_objects(0), num_groups(0), focused(root), hirarchy_active(true)
{}

Tree::~Tree()
{
	delete root;
}

GameObject * const Tree::CreateGameobject(const char* const name)
{
	GameObject* new_obj;
	num_game_objects++;
	num_nodes++;
	
	if(name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Game Object %i", num_game_objects);
		new_obj = new GameObject(focused, new_name);
	}
	else
		new_obj = new GameObject(focused, name);

	//Create Transformation
	Transform* transform = new Transform("Transformation", new_obj);
	new_obj->AddComponent(transform);

	focused = focused->AddChild(new_obj);
	return new_obj;
}

TreeNode * const Tree::CreateGroup(const char * const name)
{
	TreeNode* new_group;
	num_groups++;
	num_nodes++;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Group %i", num_groups);
		new_group = new TreeNode(nullptr, focused, new_name);
	}
	else
		new_group = new TreeNode(nullptr, focused, name);

	focused->AddChild(new_group);

	return new_group;
}

void Tree::Focus(TreeNode * new_focus)
{
	focused = new_focus;
}

void Tree::Move(TreeNode * move)
{
	move->GetPatent()->Remove(move);
	focused->AddChild(move);
}

GameObject * Tree::GetFocused() const
{
	return focused->GetData();
}

void Tree::Empty()
{
	delete root;
	root = new TreeNode(nullptr, nullptr);
	unsigned int num_nodes = 0;
	TreeNode* focused = nullptr;
}

void Tree::Draw() const
{
	root->Draw();
}

void Tree::Hirarchy()
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((focused == root) ? ImGuiTreeNodeFlags_Selected : 0);

	if (ImGui::Begin("Hirarchy", &hirarchy_active))
	{
		root->Hirarchy(focused);

		ImGui::End();
	}
}

void Tree::FocusRoot()
{
	focused = root;
}

TreeNode * Tree::GetRoot() const
{
	return root;
}
