#ifndef PREFAB
#define PREFAB

#include <vector>
#include <string>
#include "Resource.h"

struct PrefabNode
{
	std::vector<PrefabNode*> childs;
	float transform [16];
	UID mesh;
	UID material;
	std::string name;

	PrefabNode();
	~PrefabNode();

	void AddChild(const std::string& name, char** iterator);
};

struct PrefabSource
{
	PrefabNode* parent;
};

class Prefab: public Resource
{
private:
	PrefabSource* source;

public:
	Prefab(const std::string& name, const UID& uid);
	Prefab(const std::string& name, PrefabSource* source);
	~Prefab();


	bool IsLoaded() const;

	bool Inspector();
};

#endif // !PREFAB

