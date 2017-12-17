#include "imgui\imgui.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Application.h"
#include "MeshFilter.h"

MeshFilter::MeshFilter(Mesh * mesh, const GameObject* const go, bool enabled): Component(CT_MESH_FILTER, go, enabled), mesh(mesh), draw_kdt(false)
{}

MeshFilter::MeshFilter(const MeshFilter & copy, const GameObject* const go): Component(CT_MESH_FILTER, go, copy.enabled), draw_kdt(copy.draw_kdt)
{
	mesh = App->resource_manager->UseMesh(copy.mesh->GetUID(), game_object);
}

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

void MeshFilter::SaveComponent(char ** iterator) const
{
}

const std::string & MeshFilter::GetName() const
{
	return mesh->GetName();
}

void MeshFilter::StopUsingMesh(const GameObject * go)
{
	App->resource_manager->StopUsingMesh(mesh, go);
}
