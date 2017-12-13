#include "MathGeoLib\src\Math\float4x4.h"

#include "UID.h"
#include "Globals.h"

//Resources
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "PreFab.h"
#include "Animation.h"
#include "Skeleton.h"

//Components
#include "MeshFilter.h"
#include "AppliedMaterial.h"
#include "Animator.h"

#include "GameObject.h"

//Modules
#include "FileSystem.h"
#include "ResourceManager.h"
#include "Application.h"
#include "PrefabImporter.h"

PrefabImporter::PrefabImporter()
{}

PrefabImporter::~PrefabImporter()
{}

unsigned int PrefabImporter::GetFailedBefore(unsigned int pos, bool* loads) const
{
	uint ret = 0;
	for (int i = 0; i < pos; i++)
		if (loads[i] == false)
			ret++;
	return ret;
}

unsigned int PrefabImporter::GetNodeSize(const aiScene* scene, const aiNode * node, bool* mesh_loads, bool* material_loads, bool* skeleton_loads, const char* name) const
{
	// name + transform  + mesh + material + num_childs + childs
	uint ret = 0;

	if (name == nullptr)
		ret += node->mName.length + 1;
	else
		ret += strlen(name) + 1;

	ret += sizeof(float) * 16;

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		if (mesh_loads[node->mMeshes[i]])
		{
			ret += SIZE_OF_UID; //mesh
			ret += sizeof(bool) * 2;

			if (material_loads[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex] == true)
				ret += SIZE_OF_UID; //material

			if (skeleton_loads[node->mMeshes[i]] == true)
				ret += SIZE_OF_UID; //bones
		}
	}

	ret += sizeof(uint);

	ret += sizeof(uint);

	for (int i = 0; i < node->mNumChildren; i++)
		ret += GetNodeSize(scene, node->mChildren[i], mesh_loads, material_loads, skeleton_loads);

	return ret;
}

void PrefabImporter::ImportNode(const aiNode * child, char ** iterator, const aiScene * scene, const std::vector<std::pair<UID, std::string>>& materials, bool* material_loads, const std::vector<std::pair<UID, std::string>>& meshes, bool* mesh_loads, const std::vector<std::pair<UID, std::string>> skeletons, bool* skeleton_loads, const char* name)
{
	if (name == nullptr)
	{
		memcpy(*iterator, child->mName.C_Str(), child->mName.length + 1);
		*iterator += child->mName.length + 1;
	}
	else
	{
		memcpy(*iterator, name, strlen(name) + 1);
		*iterator += strlen(name) + 1;
	}

	aiMatrix4x4 mat = child->mTransformation;
	mat = mat.Transpose();
	memcpy(*iterator, &mat.a1, sizeof(float) * 16);
	*iterator += sizeof(float) * 16;

	uint num_meshes = child->mNumMeshes;
	for (int i = 0; i < num_meshes; i++)
		if (mesh_loads[child->mMeshes[i]] == false)
			num_meshes--;

	memcpy(*iterator, &num_meshes, sizeof(uint));
	*iterator += sizeof(uint);

	for (int i = 0; i < child->mNumMeshes; i++)
	{		
		if (mesh_loads[child->mMeshes[i]])
		{
			unsigned int num_mesh = child->mMeshes[i] - GetFailedBefore(child->mMeshes[i], mesh_loads);
			memcpy(*iterator, &meshes[num_mesh].first, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;

			bool has_material = material_loads[scene->mMeshes[child->mMeshes[i]]->mMaterialIndex];
			memcpy(*iterator, &has_material, sizeof(bool));
			*iterator += sizeof(bool);

			if (has_material)
			{
				unsigned int material_index = scene->mMeshes[num_mesh]->mMaterialIndex - GetFailedBefore(scene->mMeshes[num_mesh]->mMaterialIndex, material_loads);
				memcpy(*iterator, &materials[material_index].first, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
			}

			bool has_bones = skeleton_loads[child->mMeshes[i]];
			memcpy(*iterator, &has_material, sizeof(bool));
			*iterator += sizeof(bool);

			if (has_bones)
			{
				unsigned int num_rigg = child->mMeshes[i] - GetFailedBefore(child->mMeshes[i], skeleton_loads);
				memcpy(*iterator, &skeletons[num_rigg].first, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
			}
		}
	}

	memcpy(*iterator, &child->mNumChildren, sizeof(uint));
	*iterator += sizeof(uint);

	for (int i = 0; i < child->mNumChildren; i++)
		ImportNode(child->mChildren[i], iterator, scene, materials, material_loads, meshes, mesh_loads, skeletons, skeleton_loads, name);
}

GameObject* PrefabImporter::LoadChild(char ** iterator)
{
	std::string name (*iterator);
	*iterator += name.length() + 1;

	GameObject* new_game_object = new GameObject(name);

	math::float4x4 transform;
	memcpy(&transform, *iterator, sizeof(float) * 16);
	*iterator += sizeof(float) * 16;
	new_game_object->SetLocalTransform(transform);

	uint num_meshes = **iterator;
	*iterator += sizeof(uint);

	if (num_meshes == 1)
	{
		UID mesh_uid;
		memcpy(&mesh_uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Mesh* mesh = App->resource_manager->UseMesh(mesh_uid, new_game_object);
		new_game_object->AddComponent(new MeshFilter(mesh, new_game_object));

		bool has_material;
		memcpy(&has_material, *iterator, sizeof(bool));
		*iterator += sizeof(bool);

		if (has_material)
		{
			UID material_uid;
			memcpy(&material_uid, *iterator, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;
			Material* material = App->resource_manager->UseMaterial(material_uid, new_game_object);
			new_game_object->AddComponent(new AppliedMaterial(material, new_game_object));
		}

		bool has_bones;
		memcpy(&has_bones, *iterator, sizeof(bool));
		*iterator += sizeof(bool);

		if (has_bones)
		{
			UID skeleton_uid;
			memcpy(&skeleton_uid, *iterator, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;
			Skeleton* skeleton = App->resource_manager->UseSkeleton(skeleton_uid, new_game_object);
			new_game_object->AddComponent(new Animator(skeleton, new_game_object));
		}
	}
	else
	{
		for (int i = 0; i < num_meshes; i++)
		{
			char child_name[255];
			sprintf(child_name, "%s_child_%i", name.c_str(), i);

			GameObject* new_child = new GameObject(child_name);

			UID mesh_uid;
			memcpy(&mesh_uid, *iterator, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;
			Mesh* mesh = App->resource_manager->UseMesh(mesh_uid, new_child);
			new_child->AddComponent(new MeshFilter(mesh, new_child));

			bool has_material;
			memcpy(&has_material, *iterator, sizeof(bool));
			*iterator += sizeof(bool);

			if (has_material)
			{
				UID material_uid;
				memcpy(&material_uid, *iterator, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
				Material* material = App->resource_manager->UseMaterial(material_uid, new_child);
				new_child->AddComponent(new AppliedMaterial(material, new_child));
			}

			bool has_bones;
			memcpy(&has_bones, *iterator, sizeof(bool));
			*iterator += sizeof(bool);

			if (has_bones)
			{
				UID skeleton_uid;
				memcpy(&skeleton_uid, *iterator, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
				Skeleton* skeleton = App->resource_manager->UseSkeleton(skeleton_uid, new_child);
				new_child->AddComponent(new Animator(skeleton, new_child));
			}
			
			new_game_object->AddChild(new_child);
		}
	}

	uint num_childs = **iterator;
	*iterator += sizeof(uint);

	for (int i = 0; i < num_childs; i++)
		new_game_object->AddChild(LoadChild(iterator));

	return new_game_object;
}

const UID PrefabImporter::Import(const aiScene* scene, const std::vector<std::pair<UID, std::string>>& materials, bool* material_loads, const std::vector<std::pair<UID, std::string>>& meshes, bool* mesh_loads, const std::vector<std::pair<UID, std::string>> skeletons, bool* skeleton_loads, const char* name)
{
	aiNode* root_node = scene->mRootNode;

	char format[FORMAT_SIZE] = FORMAT_PREFAB;
	char* buffer = new char[FORMAT_SIZE + GetNodeSize(scene, root_node, mesh_loads, material_loads, skeleton_loads, name)];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	ImportNode(root_node, &iterator, scene, materials, material_loads, meshes, mesh_loads, skeletons, skeleton_loads, name);

	uint length = iterator - buffer;
	UID uid(buffer, length);

	if (App->file_system->SaveFile(buffer, length, App->file_system->GetPrefabs().c_str(), uid.GetAsName(), "mm") == false)
	{
		LOG("Could not save prefab correctlly");
		return UID();
	}

	delete[] buffer;

	return uid;
}

bool PrefabImporter::Load(Prefab* to_load, const PrefabLoadConfiguration* config)
{
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetPrefabs());
	path += "\\";
	path += to_load->GetUID().GetAsName();
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		to_load->SetSource(new PrefabSource(LoadChild(&iterator)));
		return true;
	}
	return false;
}
