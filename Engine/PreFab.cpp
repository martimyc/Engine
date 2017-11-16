#include "Globals.h"
#include "GameObject.h"
#include "PreFab.h"

Prefab::Prefab(const std::string & name, const UID & uid): Resource(RT_PREFAB, name, uid), source(nullptr)
{}

Prefab::Prefab(const std::string & name, PrefabSource * source) : Resource(RT_PREFAB, name), source(source)
{}

Prefab::~Prefab()
{}

GameObject * Prefab::GetRoot() const
{
	return source->root;
}

bool Prefab::IsLoaded() const
{
	return source != nullptr;
}

bool Prefab::Inspector()
{
	return false;
}

void Prefab::SetSource(PrefabSource * source)
{
	this->source = source;
}

PrefabSource::PrefabSource(GameObject* root): root(root)
{}

PrefabSource::~PrefabSource()
{
	if(root != nullptr)
		delete root;
}
