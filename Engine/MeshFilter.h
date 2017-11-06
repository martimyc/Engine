#ifndef MESH_FILTER
#define MESH_FILTER

#include "Component.h"

class Mesh;
class AppliedMaterial;

class MeshFilter : public Component
{
private:
	Mesh* mesh;
	bool draw_kdt;

public:
	MeshFilter(const char* name, Mesh* mesh, bool enabled = true);
	MeshFilter(Mesh* mesh, bool enabled = true);
	~MeshFilter();

	void Draw(const AppliedMaterial* material = nullptr) const;

	//Gets
	const Mesh* GetMesh() const;

	void RecalculateMeshKDT();

	void DrawKDT() const;

	void Inspector();
};

#endif // !MESH_FILTER

