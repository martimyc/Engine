#include "imgui\imgui.h"
#include "Globals.h"
#include "Renderer3D.h"
#include "Application.h"
#include "GameObject.h"
#include "Tree.h"

TreeNode::TreeNode(GameObject * const data, TreeNode * const parent, const char* name = nullptr) : data(data), parent(parent), name(name)
{
	if (data != nullptr && name == nullptr)
	if(name == nullptr)
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
	if(parent == nullptr)
	{
		if (ImGui::TreeNode("Root"))
		{
			int num_nodes = 0;
			static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
			int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
			
				// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
				// Node
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)num_nodes, node_flags, data->GetName().c_str());
				if (ImGui::IsItemClicked())
					node_clicked = num_nodes;
				if (node_open)
				{
					ImGui::Text("Blah blah\nBlah Blah");
					ImGui::TreePop();
				}
				/*
				else
				{
					// Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
					ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Selectable Leaf %d", i);
					if (ImGui::IsItemClicked())
						node_clicked = i;
				}*/
			
			if (node_clicked != -1)
			{
				// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
					selection_mask = (1 << node_clicked);           // Click to single-select
			}
			ImGui::PopStyleVar();
			ImGui::TreePop();
		}
	ImGui::TreePop();
}
}

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
