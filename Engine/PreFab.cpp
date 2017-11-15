#include "Globals.h"
#include "PreFab.h"

PrefabNode::PrefabNode()
{}

PrefabNode::~PrefabNode()
{
	for (std::vector<PrefabNode*>::iterator it = childs.begin(); it != childs.end(); ++it)
		delete *it;
}

void PrefabNode::AddChild(const std::string & name, char ** iterator)
{
	PrefabNode* new_child = new PrefabNode;

	new_child->name = name;

	memcpy(new_child->mesh.uid, *iterator, SIZE_OF_UID);
	*iterator += SIZE_OF_UID;

	bool has_material = **iterator;
	*iterator += sizeof(bool);

	if (has_material)
	{
		memcpy(new_child->material.uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
	}

	childs.push_back(new_child);
}

Prefab::Prefab(const std::string & name, const UID & uid): Resource(RT_PREFAB, name, uid), source(nullptr)
{}

Prefab::Prefab(const std::string & name, PrefabSource * source) : Resource(RT_PREFAB, name), source(source)
{}

Prefab::~Prefab()
{
}

bool Prefab::IsLoaded() const
{
	return source != nullptr;
}

bool Prefab::Inspector()
{
	return false;
}

