//Assets & components
#include "Transform.h"
#include "Texture.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "Material.h"
#include "AppliedMaterial.h"
#include "GameObject.h"

//importers
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

//Modules
#include "Globals.h"
#include "TextureImporter.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "ImportManager.h"

ImportManager::ImportManager(const char * name, bool start_enabled): Module(name, start_enabled)
{}

ImportManager::~ImportManager()
{}

bool ImportManager::Init()
{
	texture_importer = new TextureImporter();
	material_importer = new MaterialImporter();
	mesh_importer = new MeshImporter();
	//scene_inporter = new ImportManager();

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ImportManager::CleanUp()
{	
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool ImportManager::ImportFromPath(const std::string & path, IMPORT_TYPE type, void* imported)
{
	bool ret = true;

	std::string name(path); //this will go from full path to file name with extension in copy to assets and then to just name in load 

	if (!App->file_system->CopyToAssets(name))
	{
		LOG("Could not copy %s to assets", path.c_str());
		ret = false;
	}

	if (ret)
	{
		switch (type)
		{
		case IT_TEXTURE:
			if (texture_importer->Import(name))
			{
				imported = App->scene_manager->CreateTexture(name);
				if (!texture_importer->Load(name, *((Texture*)imported)))
				{
					LOG("Could not load %s corectlly", name.c_str());
					ret = false;
					App->scene_manager->DeleteTexture((Texture*)imported);
				}
			}
			else
			{
				LOG("Could not import %s corectlly", name.c_str());
				ret = false;
			}			
			break;
		case IT_SCENE:
			ImportScene(path);
			imported = nullptr;
			break;
		default:
			LOG("Unknown import type");
			break;
		}
	}

	return ret;
}

bool ImportManager::Load(const std::string & name, LOAD_TYPE type, void * loaded)
{
	bool ret = true;

	switch (type)
	{
	case LT_TEXTURE:
		loaded = App->scene_manager->CreateTexture(name);
		if (!texture_importer->Load(name, *((Texture*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			App->scene_manager->DeleteTexture((Texture*)loaded);
		}
		break;
	case LT_SCENE:
		//LoadScene(name); not for the time beeing
		break;
	case LT_MESH:
		loaded = App->scene_manager->CreateMesh(name.c_str());
		if (!mesh_importer->Load(name, *((Mesh*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			delete loaded;
		}
		break;
	case LT_MATERIAL:
		loaded = App->scene_manager->CreateTexture(name);
		if (!material_importer->Load(name, *((Material*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			delete loaded;
		}
		break;
	default:
		break;
	}

	return ret;
}

bool ImportManager::ImportScene(const std::string & path) const
{
	bool ret = true;

	std::string dir;
	std::string scene_name;

	size_t pos = path.find_last_of("\\");
	size_t count = path.find_last_of(".") - pos;

	if (pos != path.length())
	{
		dir = path.substr(NULL, pos + 1);
		scene_name = path.substr(pos + 1, count - 1);
	}
	else
		LOG("No dir in path");

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr)
	{
		bool* material_loads = nullptr;
		bool* mesh_loads = nullptr;

		std::vector<Material*> scene_materials;
		std::vector<Mesh*> scene_meshes;

		if (scene->HasMaterials())
		{
			scene_materials.reserve(scene->mNumMaterials);
			material_loads = new bool[scene->mNumMaterials];

			uint num_textures = 0;
			for (int i = 0; i < scene->mNumMaterials; i++)
				num_textures += scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				LOG("Importing Material %i", i);

				char ptr[255];
				sprintf(ptr, "Material_%s_%i", scene_name.c_str(), i);
				std::string material_name(ptr);

				if (material_importer->Import(scene->mMaterials[i], dir, material_name))
				{
					Material* new_material = LoadMaterial(material_name);
					if (new_material == nullptr)
					{
						LOG("Material (%i) could not be loaded correctly", i);
						ret = false;
					}
					else
					{
						material_loads[i] = true;
						scene_materials.push_back(new_material);
					}
				}
				else
				{
					LOG("Material (%i) could not be imported correctly", i);
					material_loads[i] = false;
					ret = false;
				}
			}
		}
		else
			LOG("Scene has no materials");

		//Load meshes
		if (scene->HasMeshes())
		{
			scene_meshes.reserve(scene->mNumMeshes);
			mesh_loads = new bool[scene->mNumMeshes];		

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				LOG("Loading Mesh %i", i);

				char ptr[255];
				sprintf(ptr, "%s_Mesh_%i", scene_name.c_str(), i);
				std::string mesh_name(ptr);

				if (mesh_importer->Import(scene->mMeshes[i], dir, mesh_name))
				{
					Mesh* new_mesh = LoadMesh(mesh_name);
					if (new_mesh == nullptr)
					{
						LOG("Mesh (%i) could not be loaded correctly", i);
						App->scene_manager->DeleteMesh(new_mesh);
						ret = false;
					}
					else
					{
						mesh_loads[i] = true;
						scene_meshes.push_back(new_mesh);
					}
				}
				else
					LOG("Mesh (%i) could not be imported correctly", i);
			}
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		//Load object hirarchy and add components to them
		char go_char[255];
		sprintf(go_char, "%s_GO", scene_name.c_str());
		scene_name = go_char;

		GameObject* scene_object = App->scene_manager->CreateGameObject(scene_name.c_str());

		ImportHirarchy(*scene->mRootNode, *scene, *scene_object, scene_materials, material_loads, scene_meshes, mesh_loads);

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);
		
	return ret;
}

void ImportManager::LoadCheckers()
{
	Texture* checkers = App->scene_manager->CreateTexture("Checkers");
	texture_importer->LoadCheckers(*checkers);
}

bool ImportManager::ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<Material*>& materials, bool* material_loads, const std::vector<Mesh*>& meshes, bool* mesh_loads) const
{
	bool ret = false;

	math::float4x4 new_transform;
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			new_transform[i][j] = source.mTransformation[i][j];
		}
	}

	destination.SetTransform(new_transform);

	if (source.mMeshes != NULL)
		if (source.mNumMeshes > 1)
		{
			for (unsigned int i = 0; i < source.mNumMeshes; i++)
			{
				GameObject* child = App->scene_manager->CreateGameObject(&destination);

				unsigned int mesh_num = source.mMeshes[i];

				if (mesh_loads[mesh_num] == true)
					child->AddComponent(new MeshFilter(meshes[mesh_num]));
				else
					LOG("Can not add mesh to game object, mesh didn't load correctly");

				if (material_loads[scene.mMeshes[mesh_num]->mMaterialIndex] == true && materials.size() > scene.mMeshes[mesh_num]->mMaterialIndex)
					child->AddComponent(new AppliedMaterial(materials[scene.mMeshes[mesh_num]->mMaterialIndex]));
			}
		}
		else
		{
			unsigned int mesh_num = source.mMeshes[0];

			if (mesh_loads[mesh_num] == true)
				destination.AddComponent(new MeshFilter(meshes[mesh_num]));

			if (material_loads[scene.mMeshes[mesh_num]->mMaterialIndex] == true && materials.size() > scene.mMeshes[mesh_num]->mMaterialIndex)
				destination.AddComponent(new AppliedMaterial(materials[scene.mMeshes[mesh_num]->mMaterialIndex]));
		}

	if (source.mChildren != NULL)
	{
		for (unsigned int i = 0; i < source.mNumChildren; i++)
		{
			GameObject* child;

			if (source.mChildren[i]->mName.length != 0)
			{
				std::string name(source.mChildren[i]->mName.C_Str());
				child = App->scene_manager->CreateGameObject( &destination, name.c_str());
			}
			else
				child = App->scene_manager->CreateGameObject(&destination);

			ImportHirarchy(*source.mChildren[i], scene, *child, materials, material_loads, meshes, mesh_loads);
		}
	}

	return ret;
}

Texture * ImportManager::LoadTexture(const std::string & name) const
{
	if (App->scene_manager->GetTexture(name) == nullptr)
	{
		Texture* new_texture = App->scene_manager->CreateTexture(name);
		if (texture_importer->Load(name, *new_texture) == false)
		{
			App->scene_manager->DeleteTexture(new_texture);
			LOG("Could not load texture %s", name.c_str());
			return nullptr;
		}
		return new_texture;
	}
	else
	{
		LOG("Texture already loaded");
		return App->scene_manager->GetTexture(name);
	}
}

Material* ImportManager::LoadMaterial(const std::string& name) const
{
	if (App->scene_manager->GetMaterial(name) == nullptr)
	{
		Material* new_material = App->scene_manager->CreateMaterial(name.c_str());

		if (material_importer->Load(name, *new_material) == false)
		{
			App->scene_manager->DeleteMaterial(new_material);
			LOG("Could not load material %s", name.c_str());
			return nullptr;
		}
		return new_material;
	}
	else
	{
		LOG("Material already loaded");
		return App->scene_manager->GetMaterial(name);
	}
}

Mesh * ImportManager::LoadMesh(const std::string & name) const
{
	if (App->scene_manager->GetMesh(name) == nullptr)
	{
		Mesh* new_mesh = App->scene_manager->CreateMesh(name.c_str());

		if (mesh_importer->Load(name, *new_mesh) == false)
		{
			App->scene_manager->DeleteMesh(new_mesh);
			LOG("Could not load mesh %s", name.c_str());
			return nullptr;
		}
		return new_mesh;
	}
	else
	{
		LOG("Material already loaded");
		return App->scene_manager->GetMesh(name);
	}
}
