#include "imgui\imgui.h"

//Assets
#include "Resource.h"
#include "TextureAsset.h"
#include "Texture.h"
#include "MeshAsset.h"
#include "MaterialAsset.h"

//Components
#include "Transform.h"
#include "MeshFilter.h"
#include "AppliedMaterial.h"

//GO
#include "GameObject.h"

//importers
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

//Modules
#include "Globals.h"
#include "TextureImporter.h"
#include "ResourceManager.h"
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

UPDATE_STATUS ImportManager::Update(float dt)
{
	if (importing)
	{
		ImGui::Begin("Import Settings");
		import_config->Config();
		load_config->Config();

		if (ImGui::Button("Import"))
		{
			UID uid(Import(import_path, import_type, import_config));
			Asset* new_asset = nullptr;

			ASSET_TYPE new_asset_type = AT_NO_TYPE;
			switch (import_type)
			{
			case IT_TEXTURE:
				new_asset = new TextureAsset(load_config);
				break;
			case IT_SCENE:
				//new_asset = new SceneAsset(load_config);
				break;
			default:
				LOG("Non standard import type");
				break;
			}

			App->resource_manager->AddAsset(new_asset);

			importing = false;
			import_type = IT_NO_TYPE;
			delete import_config;
			load_config = nullptr;			
		}

		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

void ImportManager::SentToImport(const std::string & path, IMPORT_TYPE type)
{
	importing = true;
	import_path = path;
	import_type = type;

	std::string format(path.substr(path.find_last_of(".")));

	switch (type)
	{
	case IT_TEXTURE:
		import_config = new TextureImportConfiguration(format);
		load_config = new TextureLoadConfiguration();
		break;
	case IT_SCENE:
		break;
	default:
		break;
	}
}

bool ImportManager::Load(Resource * to_load, const LoadConfiguration* load_config)
{
	if (to_load->IsLoaded() == false)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	switch (to_load->GetType())
	{
	case RT_TEXTURE:
		texture_importer->Load(to_load->GetSourceUID(), (TextureLoadConfiguration*)load_config);
		break;
	case RT_MESH:
		break;
	case RT_MATERIAL:
		break;
	case RT_SCENE:
		break;
	default:
		break;
	}
}

const UID ImportManager::Import(const std::string & path, IMPORT_TYPE type, const ImportConfiguration* import_config) const
{
	std::string name(path); //this will go from full path to file name with extension in copy to assets 

	if (!App->file_system->CopyToAssets(name))
	{
		LOG("Could not copy %s to assets", path.c_str());
		return UID();
	}

	UID uid;

	name = name.substr(0, name.find_last_of(".")); //take out extension

	RESOURCE_TYPE ref_type = RT_NO_TYPE;

	switch (type)
	{
	case IT_TEXTURE:
		ref_type = RT_TEXTURE;
		uid = texture_importer->Import(name, *((TextureImportConfiguration*)import_config));
		if (uid.IsNull())
		{
			LOG("Could not import %s corectlly", name.c_str());
			return UID();
		}
	case IT_SCENE:
		/*if (ImportScene(path))
			return ;
		else
		{
			LOG("Could not import %s corectlly", name.c_str());
			return UID();
		}*/
	default:
		LOG("Unknown import type");
		return UID();
	}

	return GenerateReference(name, uid, ref_type);
}

Resource * ImportManager::LoadRef(const UID & ref) const
{
	if (App->resource_manager->Exsists(ref) == true)
	{
		LOG("Texture already loaded");
		return nullptr;
	}

	Resource* new_resource = nullptr;
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetRefs());
	path += "\\";
	path += ref.uid;
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length > 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		int name_size = *iterator;
		iterator += sizeof(int);

		char* name = new char[name_size];
		memcpy(name, iterator, name_size);
		iterator += name_size + 1; // "\0"

		UID resource_uid(iterator);
		iterator += SIZE_OF_UID;

		ASSET_TYPE type = (ASSET_TYPE)*iterator;
		iterator += sizeof(int);

		switch (type)
		{
		case AT_TEXTURE:
			new_resource = new Texture(name, ref, resource_uid);
			break;
		case AT_MESH:
			break;
		case AT_MATERIAL:
			break;
		case AT_SCENE:
			break;
		default:
			break;
		}

	}

	return new_resource;
}

const UID ImportManager::GenerateReference(const std::string & name, const UID & resource_id, RESOURCE_TYPE type) const
{
	char* buffer = new char[name.length()+ sizeof(int) + SIZE_OF_UID + sizeof(int)];
	char* iterator = buffer;

	char format[FORMAT_SIZE] = FORMAT_REF;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	int name_length = name.length();
	memcpy(iterator, &name_length, sizeof(int));
	iterator += sizeof(int);

	memcpy(iterator, name.c_str(), name.length());
	iterator += name.length();

	memcpy(iterator, resource_id.uid, SIZE_OF_UID);
	iterator += SIZE_OF_UID;

	int asset_type = (int)type;
	memcpy(iterator, &asset_type, sizeof(int));
	iterator += sizeof(int);

	unsigned int length = iterator - buffer;

	UID ref_uid(buffer, length);

	if (App->file_system->SaveFile(buffer, length, LIBRARY_REFS_FOLDER, ref_uid.uid, "mm"))
		return ref_uid;
	else
		return UID();
}

TextureSource * ImportManager::LoadTexture(const UID & uid, const TextureLoadConfiguration* load_config) const
{
	TextureSource* new_texture = texture_importer->Load(uid, load_config);
	if (new_texture != nullptr)
		return new_texture;

	LOG("Could not load texture corectlly");
	delete new_texture;
	return nullptr;
}

/*
Material * ImportManager::LoadMaterial(const UID & uid, const MaterialLoadConfiguration* load_config) const
{
	//should check if loaded previouslly by uid
	Material* new_material = material_importer->Load(uid, *load_config);
	if (new_material != nullptr)
		return new_material;

	LOG("Could not load material corectlly");
	delete new_material;
	return nullptr;
}

Mesh * ImportManager::LoadMesh(const UID & uid, const MeshLoadConfiguration* load_config) const
{
	//should check if loaded previouslly by uid
	Mesh* new_mesh = mesh_importer->Load(uid, *load_config);
	if (new_mesh != nullptr)
		return new_mesh;

	LOG("Could not load mesh corectlly");
	delete new_mesh;
	return nullptr;
}*/

bool ImportManager::ImportAndLoad(const std::string & path, IMPORT_TYPE type)
{
	//TODO import return uid and load qith given uid

	Import(path, type, import_config);
	char* file;
	int length = App->file_system->LoadFileBinary(path.c_str(), &file);

	return false;
}

bool ImportManager::ImportScene(const std::string & path) const
{
	/*bool ret = true;

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
						delete new_mesh;
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

		if (scene_object == nullptr)
			ret = false;
		else
			if (ImportHirarchy(*scene->mRootNode, *scene, *scene_object, scene_materials, material_loads, scene_meshes, mesh_loads) == false)
				ret = false;

		if (ret == false)
			App->scene_manager->RemoveWithChilds(scene_object);

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);
		
	return ret;*/

	return true;
}

bool ImportManager::ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<Material*>& materials, bool* material_loads, const std::vector<Mesh*>& meshes, bool* mesh_loads) const
{
	bool ret = true;

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

				if (child == nullptr)
					return false;

				unsigned int mesh_num = source.mMeshes[i];

				/*if (mesh_loads[mesh_num] == true)
					child->AddComponent(new MeshFilter(meshes[mesh_num]));
				else
					LOG("Can not add mesh to game object, mesh didn't load correctly");*/

				if (material_loads[scene.mMeshes[mesh_num]->mMaterialIndex] == true && materials.size() > scene.mMeshes[mesh_num]->mMaterialIndex)
					child->AddComponent(new AppliedMaterial(materials[scene.mMeshes[mesh_num]->mMaterialIndex]));
			}
		}
		else
		{
			unsigned int mesh_num = source.mMeshes[0];

			/*if (mesh_loads[mesh_num] == true)
				destination.AddComponent(new MeshFilter(meshes[mesh_num]));

			if (material_loads[scene.mMeshes[mesh_num]->mMaterialIndex] == true && materials.size() > scene.mMeshes[mesh_num]->mMaterialIndex)
				destination.AddComponent(new AppliedMaterial(materials[scene.mMeshes[mesh_num]->mMaterialIndex]));*/
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

			if (child == nullptr)
				return false;
			if (ImportHirarchy(*source.mChildren[i], scene, *child, materials, material_loads, meshes, mesh_loads) == false)
				return false;
		}
	}

	return ret;
}
