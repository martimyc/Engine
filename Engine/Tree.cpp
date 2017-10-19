#include "Globals.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"
#include "Tree.h"

TreeNode::TreeNode(GameObject * const data, TreeNode * const parent) : data(data), parent(parent)
{}

TreeNode::~TreeNode()
{
	for (std::vector<TreeNode*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete (*it);
	childs.clear();
	delete data;
}

bool TreeNode::AddChild(GameObject * new_child)
{
	if (new_child != nullptr)
	{
		childs.push_back(new TreeNode(new_child, this));
		return true;
	}
	return false;
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
		App->renderer_3d->DrawGO((*it)->GetData());
	if(data != nullptr)
		App->renderer_3d->DrawGO(data);
}

TreeNode * const TreeNode::GetPatent() const
{
	return parent;
}

GameObject * const TreeNode::GetData() const
{
	return data;
}

Tree::Tree()
{}

Tree::~Tree()
{
	delete root;
}

GameObject * const Tree::CreateGameobject()
{
	if (focused != nullptr)
	{
		GameObject* new_obj = new GameObject(focused);
		focused->AddChild(new_obj);
		return new_obj;
	}
	else
	{
		LOG("Can not create game object, no focused object to be its parent");
		return nullptr;
	}
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
