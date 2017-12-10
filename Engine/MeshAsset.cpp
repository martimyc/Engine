#include "imgui\imgui.h"
#include "Resource.h"
#include "MeshAsset.h"

GLuint MeshAsset::image = 0;

MeshImportConfiguration::MeshImportConfiguration():	load_tangents(false), gen_tangents(true), load_normals(false), gen_normals(false), gen_smooth_normals(true), load_colors(false), load_uvs(true), load_bone_weights(false), kdt(false)
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
	memcpy(*iterator, &load_tangents, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &gen_tangents, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &load_normals, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &gen_normals, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &gen_smooth_normals, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &load_colors, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &load_uvs, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &load_bone_weights, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &kdt, sizeof(bool));
	*iterator += sizeof(bool);
}

void MeshImportConfiguration::MetaLoad(char ** iterator)
{
	memcpy(&load_tangents, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&gen_tangents, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&load_normals, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&gen_normals, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&gen_smooth_normals, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&load_colors, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&load_uvs, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&load_bone_weights, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&kdt, *iterator, sizeof(bool));
	*iterator += sizeof(bool);
}

unsigned int MeshImportConfiguration::GetMetaSize() const
{
	unsigned int ret = sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	ret += sizeof(bool);
	return ret;
}

bool MeshLoadConfiguration::Config()
{
	return false;
}

void MeshLoadConfiguration::MetaSave(char ** iterator) const
{}

void MeshLoadConfiguration::MetaLoad(char ** iterator)
{}

unsigned int MeshLoadConfiguration::GetMetaSize() const
{
	return 0;
}

MeshAsset::MeshAsset(Resource * resource, const ImportConfiguration * import_config, const LoadConfiguration * load_config) : Asset(RT_MESH, resource, import_config, load_config)
{}

MeshAsset::~MeshAsset()
{}

void MeshAsset::AddInstance(const GameObject * go)
{
	instances.push_back(go);
}

GLuint MeshAsset::GetImage() const
{
	return image;
}

void MeshAsset::SetImage(GLuint id)
{
	image = id;
}
