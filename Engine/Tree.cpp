#include "imgui\imgui.h"
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

TreeNode * TreeNode::Hirarchy()
{
	TreeNode * ret = nullptr;

	const char* name;
	if (data == nullptr)
		name = "Root";
	else
		name = data->GetName().c_str();

	if (ImGui::TreeNode(name))
	{
		for (std::vector<TreeNode*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			if(ret == nullptr)
				ret = (*it)->Hirarchy();
			else
				(*it)->Hirarchy();
		ImGui::TreePop();
	}

	if (ret == nullptr)
	{
		if (ImGui::IsItemClicked(0))
			return this;
		return nullptr;
	}
	return ret;
}

Tree::Tree()
{}

Tree::~Tree()
{
	delete root;
}

GameObject * const Tree::CreateGameobject(const char* const name)
{
	GameObject* new_obj;
	num_nodes++;

	if(name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Game Object %i", num_nodes);
		new_obj = new GameObject(focused, new_name);
	}
	else
		new_obj = new GameObject(focused, name);

	focused = root->AddChild(new_obj);
	return new_obj;

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
	if (ImGui::Begin("Hirarchy", &hirarchy_active))
	{
		TreeNode* new_focus = root->Hirarchy();
		if (new_focus != nullptr && new_focus != focused)
			focused = new_focus;
		ImGui::End();
	}
	if(focused != root)
		focused->GetData()->Inspector();
}

void Tree::FocusRoot()
{
	focused = root;
}

TreeNode * Tree::GetRoot() const
{
	return root;
}
