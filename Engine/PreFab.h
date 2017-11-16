#ifndef PREFAB
#define PREFAB

#include <vector>
#include <string>
#include "Resource.h"

class GameObject;

struct PrefabSource
{
	GameObject* root;

	PrefabSource(GameObject* root);
	~PrefabSource();
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

	void SetSource(PrefabSource* source);
};

#endif // !PREFAB

