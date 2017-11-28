#include "imgui\imgui.h"
#include "Mesh.h"
#include "MeshFilter.h"

MeshFilter::MeshFilter(Mesh * mesh, bool enabled): Component(CT_MESH_FILTER, enabled), mesh(mesh), draw_kdt(false)
{}

MeshFilter::MeshFilter(const MeshFilter & copy): Component(CT_MESH_FILTER, copy.enabled), draw_kdt(copy.draw_kdt), mesh(copy.mesh)
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
	if (ImGui::TreeNode("Mesh"))
	{
		ImGui::Text("Name: %s", mesh->GetName().c_str());

		mesh->Inspector();

		ImGui::TreePop();
	}
}

const std::string & MeshFilter::GetName() const
{
	return mesh->GetName();
}
