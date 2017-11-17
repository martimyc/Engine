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
	MeshFilter(Mesh* mesh, bool enabled = true);
	~MeshFilter();

	void Draw(const AppliedMaterial* material = nullptr) const;

	const Mesh* GetMesh() const;

	void Inspector();

	const std::string& GetName() const;
};

#endif // !MESH_FILTER

