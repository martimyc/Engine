#include "Globals.h"
#include "TextureImporter.h"
#include "Application.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Transformation.h"
#include "Texture.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "Mesh.h"
#include "GameObject.h"
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
		unsigned int previous_loaded_materials = App->scene_manager->NumMaterials();
		bool* material_loads = new bool[scene->mNumMaterials];

		if (scene->HasMaterials())
		{
			//Import all materials first so that we can bind them to a mesh later when we load them

			App->scene_manager->ReserveMaterialSpace(scene->mNumMaterials);

			uint num_textures = 0;
			for (int i = 0; i < scene->mNumMaterials; i++)
				num_textures += scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);

			App->scene_manager->ReserveTextureSpace(num_textures);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				LOG("Importing Material %i", i);

				char ptr[255];
				sprintf(ptr, "Material_%s_%i", scene_name.c_str(), i);
				std::string material_name(ptr);

				if (material_importer->Import(scene->mMaterials[i], dir, material_name))
				{
					if (LoadMaterial(material_name) == nullptr)
					{
						LOG("Material (%i) could not be loaded correctly", i);
						ret = false;
					}
					else
						material_loads[i] = true;
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
		
		if (scene->HasMeshes())
		{
			// Load all meshes (for now into the same game object)
			char go_char[255];
			sprintf(go_char, "%s_GO", scene_name.c_str());
			std::string go_name(go_char);

			GameObject* new_game_object = App->scene_manager->CreateGameObject(go_name.c_str());

			new_game_object->ReserveComponentSpace(scene->mNumMeshes);
			App->scene_manager->ReserveMeshSpace(scene->mNumMeshes);

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
						if (scene->HasMaterials() && material_loads[scene->mMeshes[i]->mMaterialIndex] == true)
							new_mesh->SetMaterial(previous_loaded_materials + scene->mMeshes[i]->mMaterialIndex);
						else
							LOG("Material for this mesh did not load correctly or index is non exsistent");
						new_game_object->AddComponent(new_mesh);
					}
				}
				else
					LOG("Mesh (%i) could not be imported correctly", i);
			}
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

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
