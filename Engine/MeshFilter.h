#ifndef MESH_FILTER
#define MESH_FILTER

#include <string>
#include "Component.h"

class Mesh;
class AppliedMaterial;

class MeshFilter : public Component
{
private:
	Mesh* mesh;
	bool draw_kdt;

public:
	MeshFilter(Mesh* mesh, const GameObject* const go, bool enabled = true);
	MeshFilter(const MeshFilter& copy, const GameObject* const go);
	~MeshFilter();

	void Draw(const AppliedMaterial* material = nullptr) const;

	const Mesh* GetMesh() const;

	void Inspector();

	void LoadComponent(char** iterator, const GameObject* game_object = nullptr);
	void SaveComponent(char** iterator)const;

	const std::string& GetName() const;

	void StopUsingMesh(const GameObject* go);
};

#endif // !MESH_FILTER

