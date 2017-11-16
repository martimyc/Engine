#ifndef PREFAB
#define PREFAB

#include <vector>
#include <string>
#include "Resource.h"

class GameObject;

struct PrefabSource
{
	GameObject* root;
};

class Prefab: public Resource
{
private:
	PrefabSource* source;

public:
	Prefab(const std::string& name, const UID& uid);
	Prefab(const std::string& name, PrefabSource* source);
	~Prefab();

	GameObject* GetRoot() const;

	bool IsLoaded() const;

	bool Inspector();
};

#endif // !PREFAB

