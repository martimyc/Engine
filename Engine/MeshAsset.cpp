#include "imgui\imgui.h"
#include "Resource.h"
#include "MeshAsset.h"

MeshImportConfiguration::MeshImportConfiguration():	load_tangents(false), gen_tangents(true), load_normals(false), gen_normals(false), gen_smooth_normals(true), load_colors(false), load_uvs(true), load_bone_weights(false)
{}

bool MeshImportConfiguration::Config()
{
	bool ret = false;

	if (ImGui::Checkbox("KDT", &kdt))
		ret = true;

	ImGui::Columns(2);

	if (ImGui::Checkbox("Load Tangents & Bytangents", &load_tangents))
	{
		gen_tangents = false;
		ret = true;
	}

	ImGui::NextColumn();

	if (ImGui::Checkbox("Generate Tangents & Bytangents", &gen_tangents))
	{
		load_tangents = false;
		ret = true;
	}

	ImGui::NextColumn();

	ImGui::Columns(3);

	if (ImGui::Checkbox("Load Normals", &load_normals))
	{
		gen_normals = false;
		gen_smooth_normals = false;
		ret = true;
	}

	ImGui::NextColumn();

	if (ImGui::Checkbox("Generate Normals", &gen_normals))
	{
		load_normals = false;
		gen_smooth_normals = false;
		ret = true;
	}

	ImGui::NextColumn();

	if (ImGui::Checkbox("Generate Smooth Normals", &gen_smooth_normals))
	{
		gen_normals = false;
		load_normals = false;
		ret = true;
	}

	ImGui::Columns(1);

	if (ImGui::Checkbox("Load Vertex Colors", &load_colors))
		ret = true;

	if (ImGui::Checkbox("Load Texture Coordinates", &load_uvs))
		ret = true;

	if (ImGui::Checkbox("Load Bone Weights", &load_bone_weights))
		ret = true;
	ImGui::SameLine();
	ImGui::HelpMarker("Not suported yet");

	return ret;
}

void MeshImportConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void MeshImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int MeshImportConfiguration::GetMetaSize() const
{
	return sizeof(MeshLoadConfiguration);
}

bool MeshLoadConfiguration::Config()
{
	return false;
}

void MeshLoadConfiguration::MetaSave(char ** iterator) const
{
	memcpy(*iterator, this, GetMetaSize());
	*iterator += GetMetaSize();
}

void MeshLoadConfiguration::MetaLoad(char ** iterator)
{
	memcpy(this, *iterator, GetMetaSize());
	*iterator += GetMetaSize();
}

unsigned int MeshLoadConfiguration::GetMetaSize() const
{
	return sizeof(MeshLoadConfiguration);
}

MeshAsset::MeshAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MESH, resource, import_config, load_config)
{}

MeshAsset::~MeshAsset()
{
	delete resource;
}

void MeshAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}
