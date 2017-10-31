#include "Mesh.h"
#include "MeshFilter.h"

MeshFilter::MeshFilter(const char * name, Mesh * mesh, bool enabled): Component(CT_MESH_FILTER, name, enabled), mesh(mesh)
{}

MeshFilter::MeshFilter(Mesh * mesh, bool enabled): Component(CT_MESH_FILTER, mesh->GetName(), enabled), mesh(mesh)
{}

MeshFilter::~MeshFilter()
{}

void MeshFilter::Draw(const AppliedMaterial* material) const
{
	mesh->Draw(material);
}

const Mesh * MeshFilter::GetMesh() const
{
	return mesh;
}

void MeshFilter::Inspector()
{
	mesh->Inspector();
}
