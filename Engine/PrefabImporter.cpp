#include "MathGeoLib\src\Math\float4x4.h"

#include "UID.h"
#include "Globals.h"

//Resources
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "PreFab.h"

//Components
#include "MeshFilter.h"
#include "AppliedMaterial.h"

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

unsigned int PrefabImporter::GetFailedBefore(unsigned int pos, bool* loads, unsigned int num_objects) const
{
	uint ret = 0;
	for (int i = 0; i < num_objects; i++)
		if (!loads[i])
			ret++;
	return ret;
}

unsigned int PrefabImporter::GetNodeSize(const aiNode * node, bool* mesh_loads) const
{
	// name + transform  + mesh + material + num_childs + childs

	uint ret = node->mName.length + 1;

	ret += sizeof(float) * 16;

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		if (mesh_loads[node->mMeshes[i]])
		{
			ret += SIZE_OF_UID; //mesh
			ret += sizeof(bool) + SIZE_OF_UID; //material
		}
	}

	ret += sizeof(uint);

	ret += sizeof(uint);

	for (int i = 0; i < node->mNumChildren; i++)
		ret += GetNodeSize(node->mChildren[i], mesh_loads);

	return ret;
}

void PrefabImporter::ImportNode(const aiNode * child, char ** iterator, const aiScene * scene, const std::vector<UID>& materials, bool * material_loads, const std::vector<UID>& meshes, bool * mesh_loads)
{
	memcpy(*iterator, child->mName.C_Str(), child->mName.length + 1);
	*iterator += child->mName.length + 1;

	memcpy(*iterator, &child->mTransformation, sizeof(float) * 16);
	*iterator += sizeof(float) * 16;

	uint num_meshes = child->mNumMeshes - GetFailedBefore(child->mNumMeshes, mesh_loads, child->mNumMeshes);
	memcpy(*iterator, &num_meshes, sizeof(uint));
	*iterator += sizeof(uint);

	for (int i = 0; i < child->mNumMeshes; i++)
	{
		if (mesh_loads[child->mMeshes[i]])
		{
			UID id(meshes[child->mMeshes[i] - GetFailedBefore(child->mMeshes[i], mesh_loads, scene->mNumMeshes)]);
			memcpy(*iterator, &id, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;

			bool has_material = material_loads[scene->mMeshes[child->mMeshes[i]]->mMaterialIndex];
			memcpy(*iterator, &has_material, sizeof(bool));
			*iterator += sizeof(bool);

			if (has_material)
			{
				UID id(materials[scene->mMeshes[child->mMeshes[i]]->mMaterialIndex - GetFailedBefore(scene->mMeshes[child->mMeshes[i]]->mMaterialIndex, material_loads, scene->mNumMaterials)]);
				memcpy(*iterator, &id, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
			}
		}
	}

	memcpy(*iterator, &child->mNumChildren, sizeof(uint));
	*iterator += sizeof(uint);

	for (int i = 0; i < child->mNumChildren; i++)
		ImportNode(child->mChildren[i], iterator, scene, materials, material_loads, meshes, mesh_loads);
}

GameObject* PrefabImporter::LoadChild(char ** iterator, GameObject* parent)
{
	std::string name (*iterator);
	*iterator += name.length() + 1;

	GameObject* new_game_object = new GameObject(parent, name);

	math::float4x4 transform;
	memcpy(&transform, *iterator, sizeof(float) * 16);
	*iterator += sizeof(float) * 16;
	new_game_object->SetTransform(transform);

	uint num_meshes = **iterator;
	*iterator += sizeof(uint);

	if (num_meshes == 1)
	{
		UID mesh_uid;
		memcpy(&mesh_uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Mesh* mesh = App->resource_manager->UseMesh(mesh_uid, new_game_object);
		new_game_object->AddComponent(new MeshFilter(mesh));

		bool has_material = **iterator;
		*iterator += sizeof(bool);

		if (has_material)
		{
			UID material_uid;
			memcpy(&material_uid, *iterator, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;
			Material* material = App->resource_manager->UseMaterial(material_uid, new_game_object);
			new_game_object->AddComponent(new AppliedMaterial(material));
		}
	}
	else
	{
		for (int i = 0; i < num_meshes; i++)
		{
			char child_name[255];
			sprintf(child_name, "%s_child_%i", name.c_str(), i);

			GameObject* new_child = new GameObject(new_game_object, child_name);

			UID mesh_uid;
			memcpy(&mesh_uid, *iterator, SIZE_OF_UID);
			*iterator += SIZE_OF_UID;
			Mesh* mesh = App->resource_manager->UseMesh(mesh_uid, new_child);
			new_child->AddComponent(new MeshFilter(mesh));

			bool has_material = **iterator;
			*iterator += sizeof(bool);

			if (has_material)
			{
				UID material_uid;
				memcpy(&material_uid, *iterator, SIZE_OF_UID);
				*iterator += SIZE_OF_UID;
				Material* material = App->resource_manager->UseMaterial(mesh_uid, new_child);
				new_child->AddComponent(new AppliedMaterial(material));
			}
			
			new_game_object->AddChild(new_child);
		}
	}

	uint num_childs = **iterator;
	*iterator += sizeof(uint);

	for (int i = 0; i < num_childs; i++)
	{
		new_game_object->AddChild(LoadChild(iterator, new_game_object));
	}

	return new_game_object;
}

const UID PrefabImporter::Import(const aiScene* scene, const std::vector<UID>& materials, bool* material_loads, const std::vector<UID>& meshes, bool* mesh_loads)
{
	aiNode* root_node = scene->mRootNode;

	char format[FORMAT_SIZE] = FORMAT_PREFAB;
	char* buffer = new char[FORMAT_SIZE + GetNodeSize(root_node, mesh_loads)];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	ImportNode(root_node, &iterator, scene, materials, material_loads, meshes, mesh_loads);

	uint length = iterator - buffer;
	UID uid(buffer, length);

	if (App->file_system->SaveFile(buffer, length, LIBRARY_PREFABS_FOLDER, uid.GetAsName(), "mm") == false)
	{
		LOG("Could not save prefab correctlly");
		return UID();
	}

	delete[] buffer;

	return uid;
}

bool PrefabImporter::Load(Prefab* to_load, const PrefabLoadConfiguration * config)
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

		to_load->SetSource(new PrefabSource(LoadChild(&iterator, nullptr)));
		return true;
	}
	return false;
}
