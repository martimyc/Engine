#include "imgui\imgui.h"
#include "Parson\parson.h"

#include "UID.h"

//Assets
#include "Resource.h"
#include "Texture.h"
#include "TextureAsset.h"
#include "Mesh.h"
#include "MeshAsset.h"
#include "Material.h"
#include "MaterialAsset.h"
#include "PreFab.h"
#include "PrefabAsset.h"

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
#include "PrefabImporter.h"

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
	prefab_importer = new PrefabImporter();

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ImportManager::CleanUp()
{	
	delete texture_importer;
	delete material_importer;
	delete mesh_importer;
	delete prefab_importer;

	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

UPDATE_STATUS ImportManager::Update(float dt)
{
	if (importing == false && import_type != RT_NO_TYPE)
	{
		import_type = RT_NO_TYPE;
		import_config = nullptr;
		load_config = nullptr;
	}

	if (importing)
	{
		bool image_changed = false;
		ImVec2 settings_pos;
		ImVec2 settings_size;

		if (ImGui::Begin("Import & Load", &importing))
		{
			if (ImGui::TreeNodeEx("Import Settings", ImGuiTreeNodeFlags_Framed))
			{
				if(import_config->Config())
					image_changed = true;
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Load Settings", ImGuiTreeNodeFlags_Framed))
			{
				if(load_config->Config())
					image_changed = true;
				ImGui::TreePop();
			}

			if (ImGui::Button("Import"))
			{
				UID uid(Import(import_path, import_type, import_config));
				if (uid.IsNull())
					LOG("Could not import %s correctly", import_path.c_str());

				importing = false;
				import_type = RT_NO_TYPE;
				import_config = nullptr;
				load_config = nullptr;
			}

			settings_pos = ImGui::GetWindowPos();
			settings_size = ImGui::GetWindowSize();
		}
		ImGui::End();

		if (importing)
		{
			if (image_changed)
			{
				glDeleteTextures(1, &importing_img_id);
				switch (import_type)
				{
				case RT_TEXTURE:
					texture_importer->GenerateImage(import_path, (TextureImportConfiguration*)import_config, (TextureLoadConfiguration*)load_config, importing_img_id, importing_img_width, importing_img_height);
					break;
				case RT_PREFAB:
					break;
				default:
					break;
				}
			}

			ImGui::SetNextWindowPos(ImVec2(settings_pos.x + settings_size.x, settings_pos.y));

			while (importing_img_width > settings_size.x || importing_img_height > settings_size.y)
			{
				importing_img_width = importing_img_width / 2;
				importing_img_height = importing_img_height / 2;
			}

			while (importing_img_width * 2 < settings_size.x || importing_img_height * 2 < settings_size.y)
			{
				importing_img_width = importing_img_width * 2;
				importing_img_height = importing_img_height * 2;
			}

			ImVec2 image_size(importing_img_width, importing_img_height);
			ImVec2 window_size(image_size.x, image_size.y + 30); //lil extra for the cam
			ImGui::SetNextWindowSize(window_size);

			ImGui::Begin("Image");

			ImGui::Image((void*)importing_img_id, image_size, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::End();
		}
	}

	return UPDATE_CONTINUE;
}

void ImportManager::SentToImport(const std::string & path, RESOURCE_TYPE type)
{
	importing = true;
	import_path = path;
	import_type = type;

	std::string format(path.substr(path.find_last_of(".") + 1));

	switch (type)
	{
	case RT_TEXTURE:
		import_config = new TextureImportConfiguration(format);
		load_config = new TextureLoadConfiguration();
		break;
	case RT_PREFAB:
		import_config = new SceneImportConfiguration();
		load_config = new SceneLoadConfiguration();
		break;
	default:
		break;
	}

	texture_importer->GenerateImage(import_path, (TextureImportConfiguration*)import_config, (TextureLoadConfiguration*)load_config, importing_img_id, importing_img_width, importing_img_height);
}

bool ImportManager::Load(Resource * to_load, const LoadConfiguration* load_config)
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	switch (to_load->GetType())
	{
	case RT_TEXTURE:
		texture_importer->Load(to_load->GetUID(), (TextureLoadConfiguration*)load_config);
		break;
	case RT_MESH:
		mesh_importer->Load(to_load->GetUID(), (MeshLoadConfiguration*)load_config);
		break;
	case RT_MATERIAL:
		material_importer->Load(to_load->GetUID(), App->resource_manager->GetNumMaterials(), (MaterialLoadConfiguration*) load_config);
		break;
	case RT_PREFAB:
		prefab_importer->Load(to_load->GetUID(), (PrefabLoadConfiguration*)load_config);
		break;
	default:
		break;
	}
}

const UID ImportManager::Import(const std::string & path, RESOURCE_TYPE type, const ImportConfiguration* import_config) const
{
	if (!App->file_system->CopyToAssets(import_path))
	{
		LOG("Could not copy %s to assets", import_path.c_str());
		return UID();
	}

	UID uid;

	std::string file_name_ext(GetImportFileNameWithExtension());
	std::string file_name_no_ext(GetImportFileNameNoExtension());

	switch (type)
	{
	case RT_TEXTURE:
		uid = texture_importer->Import(file_name_no_ext, (TextureImportConfiguration*)import_config);
		if (uid.IsNull())
		{
			LOG("Could not import texture from %s corectlly", path.c_str());
			return UID();
		}
		break;
	case RT_PREFAB:
		uid = ImportScene(file_name_ext, (SceneImportConfiguration*)import_config);
		if (uid.IsNull())
		{
			LOG("Could not import scene from %s corectlly", path.c_str());
			return UID();
		}
		break;
	default:
		LOG("Unknown import type");
		return UID();
	}

	if (MetaSave(file_name_ext, uid, import_type, import_config, load_config) == false)
		return UID();

	App->resource_manager->AddAsset(file_name_no_ext, uid, import_type, import_config, load_config);

	return uid;
}

bool ImportManager::MetaSave(const std::string & file, const UID & resource_id, RESOURCE_TYPE type, const ImportConfiguration * import_config, const LoadConfiguration * load_config) const
{
	char* buffer = new char[file.size() + SIZE_OF_UID + sizeof(type) + import_config->GetMetaSize() + load_config->GetMetaSize() + 10]; //+10 out when configs are done TODO
	char* iterator = buffer;

	memcpy(iterator, file.c_str(), file.length() + 1);
	iterator += file.length() + 1;

	memcpy(iterator, &resource_id, SIZE_OF_UID);
	iterator += SIZE_OF_UID;

	memcpy(iterator, &type, sizeof(type));
	iterator += sizeof(type);

	import_config->MetaSave(iterator);

	load_config->MetaSave(iterator);

	int size = iterator - buffer;

	if (App->file_system->SaveMetaFile(buffer, size, file.c_str()) == false)
		return false;

	delete[] buffer;

	return true;
}

Asset* ImportManager::MetaLoad(const std::string & file) const
{
	char* buffer = nullptr;
	unsigned int length = App->file_system->LoadMetaFile(file, &buffer);

	if (buffer != nullptr && length != 0)
	{
		char* iterator = buffer;
		std::string name(iterator);
		iterator += name.length() + 1;

		UID uid(iterator);
		iterator += SIZE_OF_UID;

		RESOURCE_TYPE type = *((RESOURCE_TYPE*)iterator);
		iterator += sizeof(type);

		Resource* new_resource = nullptr;
		ImportConfiguration* import_config = nullptr;
		LoadConfiguration* load_config = nullptr;
		Asset* new_asset = nullptr;

		switch (type)
		{
		case RT_TEXTURE:
			new_resource = new Texture(name, uid);
			import_config = new TextureImportConfiguration;
			import_config->MetaLoad(iterator);
			load_config = new TextureLoadConfiguration;
			load_config->MetaLoad(iterator);
			new_asset = new Asset(RT_TEXTURE, new_resource, import_config, load_config);
			return new_asset;
		case RT_PREFAB:
			//new_resource = new Scene(name, uid);
			break;
		}
	}
	
	LOG("Metafile %s could not be loaded correctly", file.c_str());
	return nullptr;
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


MaterialSource * ImportManager::LoadMaterial(const UID & uid, const MaterialLoadConfiguration* load_config, unsigned int priority) const
{
	//should check if loaded previouslly by uid
	MaterialSource* new_material = material_importer->Load(uid, priority, load_config);
	if (new_material != nullptr)
		return new_material;

	LOG("Could not load material corectlly");
	delete new_material;
	return nullptr;
}

MeshSource * ImportManager::LoadMesh(const UID & uid, const MeshLoadConfiguration* load_config) const
{
	//should check if loaded previouslly by uid
	MeshSource* new_mesh = mesh_importer->Load(uid, load_config);
	if (new_mesh != nullptr)
		return new_mesh;

	LOG("Could not load mesh corectlly");
	delete new_mesh;
	return nullptr;
}

const UID ImportManager::ImportScene(const std::string & file, const SceneImportConfiguration* load_config) const
{
	UID prefab_uid;

	std::string path(App->file_system->GetAssets());
	path += "\\";
	path += file;

	std::string scene_name(GetImportFileNameNoExtension());

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr)
	{
		bool* material_loads = nullptr;
		bool* mesh_loads = nullptr;

		std::vector<UID> scene_materials;
		std::vector<UID> scene_meshes;

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

				MaterialImportConfiguration i_config;
				MaterialLoadConfiguration l_config;

				UID uid(material_importer->Import(scene->mMaterials[i], &i_config));

				if (uid.IsNull() == false)
				{
					material_loads[i] = true;
					scene_materials.push_back(uid);
					App->resource_manager->AddAsset(material_name, uid, RT_MATERIAL, &i_config, &l_config);
				}
				else
				{
					LOG("Material (%i) could not be imported correctly", i);
					material_loads[i] = false;
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

				MeshImportConfiguration i_config;
				MeshLoadConfiguration l_config;

				UID uid(mesh_importer->Import(scene->mMeshes[i])); //TODO import should take import config

				mesh_loads[i] = true;
				scene_meshes.push_back(uid);

				if (uid.IsNull() == false)
				{
					mesh_loads[i] = true;
					scene_meshes.push_back(uid);
					App->resource_manager->AddAsset(mesh_name, uid, RT_MESH, &i_config, &l_config);
				}
				else
				{
					LOG("Mesh (%i) could not be imported correctly", i);
					material_loads[i] = false;
				}
			}
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		if (scene->mRootNode != nullptr)
		{
			PrefabImportConfiguration i_config;
			PrefabLoadConfiguration l_config;

			prefab_uid = prefab_importer->Import(scene, scene_materials, material_loads, scene_meshes, mesh_loads, &i_config);

			if (prefab_uid.IsNull() == false)
				App->resource_manager->AddAsset(scene_name, prefab_uid, RT_PREFAB, &i_config, &l_config);
			else
			{
				LOG("Scene could not be loaded as prefab");
				prefab_uid = UID();
			}
		}

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene %s", path.c_str());
	}
		
	return prefab_uid;
}

bool ImportManager::ImportHirarchy(const aiNode & source, const aiScene& scene, GameObject & destination, const std::vector<UID>& materials, bool* material_loads, const std::vector<UID>& meshes, bool* mesh_loads) const
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

				if (mesh_loads[mesh_num] == true)
					child->AddComponent(new MeshFilter((Mesh*)App->resource_manager->Use(meshes[mesh_num], child), "AluAcbar"));
				else
					LOG("Can not add mesh to game object, mesh didn't load correctly");

				/*if (material_loads[scene.mMeshes[mesh_num]->mMaterialIndex] == true && materials.size() > scene.mMeshes[mesh_num]->mMaterialIndex)
					child->AddComponent(new AppliedMaterial(materials[scene.mMeshes[mesh_num]->mMaterialIndex]));*/
			}
		}
		else
		{
			unsigned int mesh_num = source.mMeshes[0];
			destination.AddComponent(new MeshFilter((Mesh*)App->resource_manager->Use(meshes[mesh_num], &destination), "AluAcbar"));

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

const std::string ImportManager::GetImportFileNameNoExtension() const
{
	size_t start = import_path.find_last_of("\\");
	size_t count = import_path.find_last_of(".") - start;
	return import_path.substr(start + 1, count - 1);
}

const std::string ImportManager::GetImportFileNameWithExtension() const
{
	size_t start = import_path.find_last_of("\\");
	return import_path.substr(start + 1);
}

const UID ImportManager::ImportClient::Import(const ImportManager * importer, const std::string & path, RESOURCE_TYPE type, const ImportConfiguration * import_config)
{
	return importer->Import(path, type, import_config);
}
