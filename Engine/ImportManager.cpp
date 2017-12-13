#include <memory>

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
#include "Animation.h"
#include "AnimationAsset.h"
#include "Skeleton.h"
#include "SkeletonAsset.h"
#include "AssetDirectory.h"

//Components
#include "Transform.h"
#include "MeshFilter.h"
#include "AppliedMaterial.h"

//GO
#include "GameObject.h"

//importers
#include "TextureImporter.h"
#include "Input.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "PrefabImporter.h"
#include "AnimationImporter.h"
#include "SkeletonImporter.h"

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
	anim_importer = new AnimationImporter();
	skeleton_importer = new SkeletonImporter();
	
	Assimp::DefaultLogger* logger = (Assimp::DefaultLogger*)Assimp::DefaultLogger::get();
	logger->create();

	return true;
}

bool ImportManager::CleanUp()
{	
	delete texture_importer;
	delete material_importer;
	delete mesh_importer;
	delete prefab_importer;
	delete anim_importer;
	delete skeleton_importer;

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

			if (import_type == RT_TEXTURE)
			{
				if (ImGui::TreeNodeEx("Load Settings", ImGuiTreeNodeFlags_Framed))
				{
					if (load_config->Config())
						image_changed = true;
					ImGui::TreePop();
				}
			}

			if (ImGui::Button("Import"))
			{
				UID uid(Import(import_path, import_type, import_config, load_config));
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

		if (importing && import_type == RT_TEXTURE)
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
		load_config = nullptr;
		break;
	default:
		break;
	}

	texture_importer->GenerateImage(import_path, (TextureImportConfiguration*)import_config, (TextureLoadConfiguration*)load_config, importing_img_id, importing_img_width, importing_img_height);
}

bool ImportManager::LoadMaterial(Material * to_load, const MaterialLoadConfiguration* load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	return material_importer->Load(to_load, App->resource_manager->GetNewMaterialPriority(), load_config);
}

bool ImportManager::LoadTexture(Texture * to_load, const TextureLoadConfiguration * load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	return texture_importer->Load(to_load, load_config);
}

bool ImportManager::LoadMesh(Mesh * to_load, const MeshLoadConfiguration* load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	bool ret = mesh_importer->Load(to_load, load_config);

	//to_load->RecalculateKDT();

	return ret;
}

bool ImportManager::LoadPrefab(Prefab * to_load, const PrefabLoadConfiguration* load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}
	return prefab_importer->Load(to_load, load_config);
}

bool ImportManager::LoadSkeleton(Skeleton * to_load, const SkeletonLoadConfiguration * load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}
	return skeleton_importer->Load(to_load, load_config);
}

bool ImportManager::LoadAnimation(Animation * to_load, const AnimationLoadConfiguration * load_config) const
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}
	return anim_importer->Load(to_load, load_config);
}

GLuint ImportManager::GenerateButtonImage(const std::string & relative_path)
{
	std::string path(App->file_system->GetAssets());
	path += "\\";
	path += relative_path;
	GLuint ret = 0;
	TextureLoadConfiguration l_config;
	TextureImportConfiguration i_config;
	l_config.anysotropy = false;
	uint width = 0;
	uint height = 0;
	if (texture_importer->GenerateImage(path, &i_config, &l_config, ret, width, height) == false)
		LOG("Could not generate image %s", relative_path.c_str());
	return ret;
}

const UID ImportManager::Import(const std::string & path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config, AssetDirectory* dir) const
{
	std::string new_path;
	UID uid;
	std::string file_name_ext(path.substr(path.find_last_of("\\") + 1));
	std::string file_name_no_ext(file_name_ext.substr(0, file_name_ext.find_last_of(".")));

	if (dir == nullptr)
	{
		if (App->resource_manager->CopyFileToCurrentDir(path) == false)
		{
			LOG("Could not copy %s to current directory", path.c_str());
			return UID();
		}
		new_path = App->resource_manager->GetCurrentDirPath() + "\\" + file_name_ext;
	}
	else
	{
		if(App->file_system->CopyTo(path, dir->GetPath()) == false)
		{
			LOG("Could not copy %s to current directory", path.c_str());
			return UID();
		}
		new_path = dir->GetPath() + "\\" + file_name_ext;
	}

	switch (type)
	{
	case RT_TEXTURE:
		uid = texture_importer->Import(new_path, (TextureImportConfiguration*)import_config);
		if (uid.IsNull())
		{
			LOG("Could not import texture from %s corectlly", path.c_str());
			return UID();
		}
		if (TextureMetaSave(new_path, uid, (TextureImportConfiguration*)import_config, (TextureLoadConfiguration*)load_config) == false)
			return UID();
		break;
	case RT_PREFAB:
		uid = ImportScene(path, (SceneImportConfiguration*)import_config, dir);
		delete import_config;
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

	if(type != RT_PREFAB)
		if(dir == nullptr)
			App->resource_manager->AddAsset(file_name_no_ext, uid, type, import_config, load_config);
		else
			dir->AddAsset(file_name_no_ext, uid, type, import_config, load_config);

	return uid;
}

bool ImportManager::TextureMetaSave(const std::string & full_path, const UID & resource_id, const TextureImportConfiguration * import_config, const TextureLoadConfiguration * load_config) const
{
	unsigned int buff_size = full_path.size() + 1;
	buff_size += SIZE_OF_UID;
	buff_size += sizeof(time_t);
	buff_size += sizeof(RESOURCE_TYPE);
	buff_size += import_config->GetMetaSize();
	buff_size += load_config->GetMetaSize();

	char* buffer = new char[buff_size];
	char* iterator = buffer;

	time_t last_modified = App->file_system->GetTimeStamp(full_path);
	memcpy(iterator, &last_modified, sizeof(time_t));
	iterator += sizeof(time_t);

	memcpy(iterator, full_path.c_str(), full_path.length() + 1);
	iterator += full_path.length() + 1;

	memcpy(iterator, &resource_id, SIZE_OF_UID);
	iterator += SIZE_OF_UID;

	RESOURCE_TYPE type = RT_TEXTURE;
	memcpy(iterator, &type, sizeof(RESOURCE_TYPE));
	iterator += sizeof(RESOURCE_TYPE);

	import_config->MetaSave(&iterator);

	load_config->MetaSave(&iterator);

	int size = iterator - buffer;

	std::string name(full_path.substr(full_path.find_last_of("\\") + 1));
	std::string dir(full_path.substr(0, full_path.find_last_of("\\")));

	if (App->file_system->SaveMetaFile(buffer, size, name.c_str(), dir.c_str()) == false)
		return false;

	delete[] buffer;

	return true;
}

bool ImportManager::SceneMetaSave(const std::string & full_path, const std::vector<std::pair<UID, std::string>>& mesh_uids, const std::vector<std::pair<UID, std::string>>& material_uids, const std::vector<std::pair<UID, std::string>>& skeletons, const std::vector<std::pair<UID, std::string>>& animations, const UID & prefab_id, const SceneImportConfiguration * import_config) const
{
	unsigned int buff_size = full_path.size() + 1;
	buff_size += SIZE_OF_UID;
	buff_size += sizeof(time_t);
	buff_size += sizeof(RESOURCE_TYPE);
	buff_size += import_config->GetMetaSize();

	buff_size += sizeof(unsigned int);
	buff_size += mesh_uids.size() * SIZE_OF_UID;
	for (std::vector<std::pair<UID, std::string>>::const_iterator it = mesh_uids.begin(); it != mesh_uids.end(); ++it)
		buff_size += it->second.size() + 1;

	buff_size += sizeof(unsigned int);
	buff_size += material_uids.size() * SIZE_OF_UID;
	for (std::vector<std::pair<UID, std::string>>::const_iterator it = material_uids.begin(); it != material_uids.end(); ++it)
		buff_size += it->second.size() + 1;

	buff_size += sizeof(unsigned int);
	buff_size += skeletons.size() * SIZE_OF_UID;
	for (std::vector<std::pair<UID, std::string>>::const_iterator it = skeletons.begin(); it != skeletons.end(); ++it)
		buff_size += it->second.size() + 1;

	buff_size += sizeof(unsigned int);
	buff_size += animations.size() * SIZE_OF_UID;
	for (std::vector<std::pair<UID, std::string>>::const_iterator it = animations.begin(); it != animations.end(); ++it)
		buff_size += it->second.size() + 1;

	char* buffer = new char[buff_size];
	char* iterator = buffer;

	time_t last_modified = App->file_system->GetTimeStamp(full_path);
	memcpy(iterator, &last_modified, sizeof(time_t));
	iterator += sizeof(time_t);

	memcpy(iterator, full_path.c_str(), full_path.length() + 1);
	iterator += full_path.length() + 1;

	memcpy(iterator, &prefab_id, SIZE_OF_UID);
	iterator += SIZE_OF_UID;

	RESOURCE_TYPE type = RT_PREFAB;
	memcpy(iterator, &type, sizeof(RESOURCE_TYPE));
	iterator += sizeof(RESOURCE_TYPE);

	import_config->MetaSave(&iterator);

	unsigned int num_meshes = mesh_uids.size();
	memcpy(iterator, &num_meshes, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	for (int i = 0; i < num_meshes; i++)
	{
		memcpy(iterator, &mesh_uids[i].first, SIZE_OF_UID);
		iterator += SIZE_OF_UID;

		memcpy(iterator, mesh_uids[i].second.c_str(), mesh_uids[i].second.size() + 1);
		iterator += mesh_uids[i].second.size() + 1;
	}

	unsigned int num_materials = material_uids.size();
	memcpy(iterator, &num_materials, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	for (int i = 0; i < num_materials; i++)
	{
		memcpy(iterator, &material_uids[i].first, SIZE_OF_UID);
		iterator += SIZE_OF_UID;

		memcpy(iterator, material_uids[i].second.c_str(), material_uids[i].second.size() + 1);
		iterator += material_uids[i].second.size() + 1;
	}

	//---
	unsigned int num_riggs = skeletons.size();
	memcpy(iterator, &num_riggs, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	for (int i = 0; i < num_riggs; i++)
	{
		memcpy(iterator, &skeletons[i].first, SIZE_OF_UID);
		iterator += SIZE_OF_UID;

		memcpy(iterator, skeletons[i].second.c_str(), skeletons[i].second.size() + 1);
		iterator += skeletons[i].second.size() + 1;
	}

	unsigned int num_animations = animations.size();
	memcpy(iterator, &num_animations, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	for (int i = 0; i < num_animations; i++)
	{
		memcpy(iterator, &animations[i].first, SIZE_OF_UID);
		iterator += SIZE_OF_UID;

		memcpy(iterator, animations[i].second.c_str(), animations[i].second.size() + 1);
		iterator += animations[i].second.size() + 1;
	}
	//---

	int size = iterator - buffer;

	std::string name(full_path.substr(full_path.find_last_of("\\") + 1));
	std::string dir(full_path.substr(0, full_path.find_last_of("\\")));

	if (App->file_system->SaveMetaFile(buffer, size, name.c_str(), dir.c_str()) == false)
		return false;

	delete[] buffer;

	return true;
}

void ImportManager::MetaLoad(const std::string & file, AssetDirectory* dir) const
{
	char* buffer = nullptr;
	unsigned int length = App->file_system->LoadFileBinary(dir->GetPath() + "\\" + file, &buffer);

	if (buffer != nullptr && length != 0)
	{
		char* iterator = buffer;

		iterator += sizeof(time_t);

		std::string file_path(iterator);
		iterator += file_path.length() + 1;

		size_t count = file_path.find_first_of(".") - 1 - file_path.find_last_of("\\");
		std::string name(file_path.substr(file_path.find_last_of("\\") + 1, count));

		UID uid(iterator);
		iterator += SIZE_OF_UID;

		RESOURCE_TYPE type = *((RESOURCE_TYPE*)iterator);
		iterator += sizeof(RESOURCE_TYPE);

		ImportConfiguration* import_config = nullptr;
		LoadConfiguration* load_config = nullptr;

		switch (type)
		{
		case RT_TEXTURE:
			import_config = new TextureImportConfiguration;
			import_config->MetaLoad(&iterator);
			load_config = new TextureLoadConfiguration;
			load_config->MetaLoad(&iterator);
			dir->AddAsset(name, uid, RT_TEXTURE, import_config, load_config);
			break;
		case RT_PREFAB:
			import_config = new SceneImportConfiguration;
			import_config->MetaLoad(&iterator);
			LoadScene(dir, &iterator,(SceneImportConfiguration*) import_config, name);
			dir->AddAsset(name, uid, RT_PREFAB, ((SceneImportConfiguration*)import_config)->prefab_import_config, ((SceneImportConfiguration*)import_config)->prefab_load_config);
			break;
		}
	}
	else
		LOG("Metafile %s could not be loaded correctly", name.c_str());

	delete[] buffer;
}

void ImportManager::ReImportWithMeta(time_t file_last_mod, const std::string & meta_file, AssetDirectory* dir) const
{
	char* buffer = nullptr;
	unsigned int length = App->file_system->LoadFileBinary(meta_file, &buffer);

	if (buffer != nullptr && length != 0)
	{
		char* iterator = buffer;

		time_t last_mod;
		memcpy(&last_mod, iterator, sizeof(time_t));
		iterator += sizeof(time_t);

		if (last_mod != file_last_mod)
		{
			std::string file_path(iterator);
			iterator += file_path.length() + 1;

			size_t count = file_path.find_first_of(".") - 1 - file_path.find_last_of("\\");
			std::string name(file_path.substr(file_path.find_last_of("\\") + 1, count));

			UID uid(iterator);
			iterator += SIZE_OF_UID;

			dir->DeleteAsset(uid);

			RESOURCE_TYPE type = *((RESOURCE_TYPE*)iterator);
			iterator += sizeof(RESOURCE_TYPE);

			ImportConfiguration* import_config = nullptr;
			LoadConfiguration* load_config = nullptr;

			std::string to_erase;
			std::string real_file;

			switch (type)
			{
			case RT_TEXTURE:
				import_config = new TextureImportConfiguration;
				import_config->MetaLoad(&iterator);

				load_config = new TextureLoadConfiguration;
				load_config->MetaLoad(&iterator);

				to_erase = App->file_system->GetTextures() + "\\" + uid.GetAsName() + ".dds";
				if (App->file_system->EraseFile(to_erase) == false)
					LOG("Could not delete %s", to_erase.c_str());

				real_file = meta_file.substr(0, meta_file.find_last_of("."));

				if (App->file_system->EraseFile(meta_file) == false)
					LOG("Could not delete %s", meta_file.c_str());

				Import(real_file, RT_TEXTURE, import_config, load_config, dir);
				break;

			case RT_PREFAB:
				import_config = new SceneImportConfiguration;
				import_config->MetaLoad(&iterator);
				DeleteSceneFiles(&iterator);

				to_erase = App->file_system->GetPrefabs() + "\\" + uid.GetAsName() + ".mm";
				if (App->file_system->EraseFile(to_erase) == false)
					LOG("Could not delete %s", to_erase.c_str());

				real_file = meta_file.substr(0, meta_file.find_last_of("."));

				if (App->file_system->EraseFile(meta_file) == false)
					LOG("Could not delete %s", meta_file.c_str());

				ImportScene(real_file, (SceneImportConfiguration*)import_config, dir);
				break;
			}
		}
		else
			return;
	}
	else
		LOG("Metafile %s could not be loaded correctly", name.c_str());
	delete[] buffer;
}

const UID ImportManager::ImportScene(const std::string & path, const SceneImportConfiguration* import_config, AssetDirectory* dir) const
{
	UID prefab_uid;

	std::string scene_name(path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") -1));
	std::string scene_dir(path.substr(0, path.find_last_of("\\") + 1)); // +1 to include \\

	Assimp::Importer* importer = new Assimp::Importer;

	//Config
	unsigned int flags =  aiProcess_ValidateDataStructure | aiProcess_ImproveCacheLocality | aiProcess_FindInvalidData | aiProcess_FindInstances;
	//aiProcess_TransformUVCoords  as long as there are no uv transforms
	//aiProcess_FixInfacingNormals result might not always be correct

	uint remove_components = 0;

	if (import_config->mesh_import_config->load_colors == false)
		remove_components |= aiComponent_COLORS;
	if (import_config->mesh_import_config->load_tangents == false)
		remove_components |= aiComponent_TANGENTS_AND_BITANGENTS;
	if (import_config->mesh_import_config->load_normals == false)
		remove_components |= aiComponent_NORMALS;		
	if (import_config->mesh_import_config->load_uvs == false)
		remove_components |= aiComponent_TEXCOORDS;
	else
		flags |= aiProcess_GenUVCoords;
	if (import_config->skeleton_import_config->load_bone_weights == false)
		remove_components |= aiComponent_BONEWEIGHTS;
	if (import_config->include_animations == false)
		remove_components |= aiComponent_ANIMATIONS;
	if (import_config->material_import_config->include_textures == false)
		remove_components |= aiComponent_TEXTURES;
	if (import_config->include_lights == false)
		remove_components |= aiComponent_LIGHTS;
	if (import_config->include_cameras == false)
		remove_components |= aiComponent_CAMERAS;
	if (import_config->include_meshes == false)
		remove_components |= aiComponent_MESHES;
	else
		flags |= aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate;
	if (import_config->include_materials == false)
		remove_components |= aiComponent_MATERIALS;
	else
		flags |= aiProcess_RemoveRedundantMaterials;

	if (remove_components != 0)
	{
		flags |= aiProcess_RemoveComponent;
		importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, remove_components);
	}
	
	if (import_config->material_import_config->include_textures && import_config->material_import_config->texture_import_config->transform_uvs)
		flags |= aiProcess_TransformUVCoords;
	//Mesh
	if (import_config->mesh_import_config->gen_normals)
		flags |= aiProcess_GenNormals;
	if (import_config->mesh_import_config->gen_smooth_normals)
		flags |= aiProcess_GenSmoothNormals;
	if (import_config->mesh_import_config->load_normals && import_config->mesh_import_config->fix_inward_normals)
		flags |= aiProcess_FixInfacingNormals;
	//Animation
	if (import_config->skeleton_import_config->limit_bone_weights)
		flags |= aiProcess_LimitBoneWeights;
	if (import_config->skeleton_import_config->split_by_bone_count)
		flags |= aiProcess_SplitByBoneCount;
	if (import_config->skeleton_import_config->debone)
	{
		flags |= aiProcess_Debone;
		importer->SetPropertyInteger(AI_CONFIG_PP_DB_THRESHOLD, import_config->skeleton_import_config->debone_threshold);
	}

	//Prefab
	if (import_config->prefab_import_config->split_large_meshes)
		flags |= aiProcess_SplitLargeMeshes;
	if (import_config->prefab_import_config->pre_transform)
		flags |= aiProcess_PreTransformVertices;
	if (import_config->prefab_import_config->split_large_meshes)
		flags |= aiProcess_SplitLargeMeshes;
	if (import_config->prefab_import_config->optimize_graph)
		flags |= aiProcess_OptimizeGraph;
	if (import_config->prefab_import_config->optimize_meshes)
		flags |= aiProcess_OptimizeMeshes;
	if (import_config->prefab_import_config->sort_by_type)
	{
		flags |= aiProcess_SortByPType;
		importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	}
	else
		importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, 1);
	//---

	const aiScene* scene = importer->ReadFile(path.c_str(), flags);

	if (scene != nullptr)
	{
		//warnings
		if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			LOG("Scene is incomplete");
		if (scene->mFlags & AI_SCENE_FLAGS_VALIDATED)
			LOG("Scene validated without warnings");
		if (scene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING)
			LOG("Scene validated with warnings");
		if (scene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING)
			LOG("Scene validated with warnings");

		bool* material_loads = nullptr;
		bool* mesh_loads = nullptr;
		bool* skeleton_loads = nullptr;
		bool* animation_loads = nullptr;

		std::vector<std::pair<UID, std::string>> scene_materials;
		std::vector<std::pair<UID, std::string>> scene_meshes;
		std::vector<std::pair<UID, std::string>> scene_skeletons;
		std::vector<std::pair<UID, std::string>> scene_animations;

		if (scene->HasMaterials())
		{
			scene_materials.reserve(scene->mNumMaterials);
			material_loads = new bool[scene->mNumMaterials];

			//Materials
			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
				{
					LOG("Importing Material %i", i);

					char ptr[255];
					sprintf(ptr, "Material_%s_%i", scene_name.c_str(), i);
					std::string material_name(ptr);

					UID uid(material_importer->Import(scene_dir, scene->mMaterials[i], import_config->material_import_config, dir));

					if (uid.IsNull() == false)
					{
						material_loads[i] = true;
						scene_materials.push_back(std::make_pair(uid,ptr));

						if(dir == nullptr)
							App->resource_manager->AddAsset(material_name, uid, RT_MATERIAL, new MaterialImportConfiguration(*import_config->material_import_config), new MaterialLoadConfiguration(*import_config->material_load_config));
						else
							dir->AddAsset(material_name, uid, RT_MATERIAL, new MaterialImportConfiguration(*import_config->material_import_config), new MaterialLoadConfiguration(*import_config->material_load_config));
					}
					else
					{
						LOG("Material (%i) could not be imported correctly", i);
						material_loads[i] = false;
					}
				}
				else
				{
					LOG("Material (%i) was empty", i);
					material_loads[i] = false;
				}
			}
		}
		else
			LOG("Scene has no materials");

		//Meshes
		if (scene->HasMeshes())
		{
			scene_meshes.reserve(scene->mNumMeshes);
			scene_skeletons.reserve(scene->mNumMeshes);
			mesh_loads = new bool[scene->mNumMeshes];
			skeleton_loads = new bool[scene->mNumMeshes];

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				LOG("Loading Mesh %i", i);

				std::string mesh_name(scene->mMeshes[i]->mName.C_Str());
				if (mesh_name.substr(0, mesh_name.find_first_of(".")) == "Untitled" || mesh_name == "")
				{
					char ptr[255];
					sprintf(ptr, "%s_Mesh_%i", scene_name.c_str(), i);
					mesh_name = ptr;
				}

				UID uid(mesh_importer->Import(scene->mMeshes[i], import_config->mesh_import_config));

				if (uid.IsNull() == false)
				{
					mesh_loads[i] = true;
					scene_meshes.push_back(std::make_pair(uid, mesh_name.c_str()));
					if(dir == nullptr)
						App->resource_manager->AddAsset(mesh_name, uid, RT_MESH, new MeshImportConfiguration(*import_config->mesh_import_config), new MeshLoadConfiguration(*import_config->mesh_load_config));
					else
						dir->AddAsset(mesh_name, uid, RT_MESH, new MeshImportConfiguration(*import_config->mesh_import_config), new MeshLoadConfiguration(*import_config->mesh_load_config));
				}
				else
				{
					LOG("Mesh (%i) could not be imported correctly", i);
					mesh_loads[i] = false;
				}

				//Skeletons
				if (scene->mMeshes[i]->HasBones())
				{
					LOG("Loading Rigg for mesh %s", mesh_name.c_str());

					char ptr[255];
					sprintf(ptr, "%s_Rigg", mesh_name.c_str());
					std::string rigg_name(ptr);
					
					UID uid(skeleton_importer->Import(scene->mMeshes[i]->mBones, scene->mRootNode, scene->mMeshes[i]->mNumBones));

					if (uid.IsNull() == false)
					{
						skeleton_loads[i] = true;
						scene_skeletons.push_back(std::make_pair(uid, rigg_name.c_str()));
						if (dir == nullptr)
							App->resource_manager->AddAsset(rigg_name, uid, RT_SKELETON, new SkeletonImportConfiguration(*import_config->skeleton_import_config), new SkeletonLoadConfiguration(*import_config->skeleton_load_config));
						else
							dir->AddAsset(rigg_name, uid, RT_SKELETON, new SkeletonImportConfiguration(*import_config->skeleton_import_config), new SkeletonLoadConfiguration(*import_config->skeleton_load_config));
					}
					else
					{
						LOG("Skeleton (%s) could not be imported correctly", rigg_name.c_str());
						skeleton_loads[i] = false;
					}
				}
				else
					skeleton_loads[i] = false;
			}			
		}

		//Animations
		if (scene->HasAnimations())
		{
			animation_loads = new bool[scene->mNumAnimations];
			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				std::string anim_name(scene->mAnimations[i]->mName.C_Str());
				if (anim_name.substr(0, anim_name.find_first_of(".")) == "Untitled" || anim_name == "")
				{
					char ptr[255];
					sprintf(ptr, "%s_Animation_%i", scene_name.c_str(), i);
					anim_name = ptr;
				}

				LOG("Loading Animation %s", anim_name.c_str());

				UID uid(anim_importer->Import(scene->mAnimations[i], import_config->anim_import_config));

				if (uid.IsNull() == false)
				{
					animation_loads[i] = true;
					scene_animations.push_back(std::make_pair(uid, anim_name.c_str()));
					if (dir == nullptr)
						App->resource_manager->AddAsset(anim_name, uid, RT_ANIMATION, new AnimationImportConfiguration(*import_config->anim_import_config), new AnimationLoadConfiguration(*import_config->anim_load_config));
					else
						dir->AddAsset(anim_name, uid, RT_ANIMATION, new AnimationImportConfiguration(*import_config->anim_import_config), new AnimationLoadConfiguration(*import_config->anim_load_config));
				}
				else
				{
					LOG("Animation (%s) could not be imported correctly", anim_name.c_str());
					animation_loads[i] = false;
				}
			}
		}

		//Hirarchy
		if (scene->mRootNode != nullptr)
		{
			prefab_uid = prefab_importer->Import(scene, scene_materials, material_loads, scene_meshes, mesh_loads, scene_skeletons, skeleton_loads);

			if (prefab_uid.IsNull() == false)
			{
				if(dir == nullptr)
					App->resource_manager->AddAsset(scene_name, prefab_uid, RT_PREFAB, import_config->prefab_import_config, import_config->prefab_load_config);
				else
					dir->AddAsset(scene_name, prefab_uid, RT_PREFAB, import_config->prefab_import_config, import_config->prefab_load_config);
			}
			else
			{
				LOG("Scene could not be loaded as prefab");
				prefab_uid = UID();
			}
		}

		std::string file_name_ext(path.substr(path.find_last_of("\\")));

		if(dir == nullptr)
			SceneMetaSave(App->resource_manager->GetCurrentDirPath() + file_name_ext, scene_meshes, scene_materials, scene_skeletons, scene_animations, prefab_uid, import_config);
		else
			SceneMetaSave(dir->GetPath() + file_name_ext, scene_meshes, scene_materials, scene_skeletons, scene_animations, prefab_uid, import_config);

		delete[] material_loads;
		delete[] mesh_loads;
		delete[] skeleton_loads;
		delete[] animation_loads;
		delete importer;
	}
	else
	{
		LOG("Error %s loading scene %s", aiGetErrorString(), path.c_str());
		std::string file_name(path.substr(path.find_last_of("\\") + 1));
		App->file_system->EraseFile(App->resource_manager->GetCurrentDirPath() + "\\" + file_name);
	}
	
	return prefab_uid;
}

void ImportManager::LoadScene(AssetDirectory* dir, char ** iterator, const SceneImportConfiguration* config, const std::string& scene_name) const
{
	unsigned int num_meshes;
	memcpy( &num_meshes, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_meshes; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		dir->AddAsset(name, uid, RT_MESH, config->mesh_import_config, config->mesh_load_config);
	}

	unsigned int num_materials;
	memcpy(&num_materials, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_materials; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		dir->AddAsset(name, uid, RT_MATERIAL, config->mesh_import_config, config->mesh_load_config);
	}

	unsigned int num_riggs;
	memcpy(&num_riggs, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_riggs; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		dir->AddAsset(name, uid, RT_SKELETON, config->skeleton_import_config, config->skeleton_load_config);
	}

	unsigned int num_animations;
	memcpy(&num_animations, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_animations; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		dir->AddAsset(name, uid, RT_ANIMATION, config->anim_import_config, config->anim_load_config);
	}
}

void ImportManager::EraseDummyNodes(aiNode * root_node) const
{

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

void ImportManager::DeleteSceneFiles(char ** iterator) const
{
	unsigned int num_meshes;
	memcpy(&num_meshes, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_meshes; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		std::string to_delete(App->file_system->GetMeshes() + "\\" + uid.GetAsName());
		App->file_system->EraseFile(to_delete);
	}

	unsigned int num_materials;
	memcpy(&num_materials, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_materials; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;

		std::string name(*iterator);
		*iterator += name.size() + 1;

		std::string to_delete(App->file_system->GetMaterials() + "\\" + uid.GetAsName());
		App->file_system->EraseFile(to_delete);
	}
}

const UID ImportManager::ImportClient::Import(const ImportManager * importer, const std::string & path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config, AssetDirectory* dir)
{
	return importer->Import(path, type, import_config, load_config, dir);
}

void ImportManager::ImportClient::StraightImport(const std::string & file_path, AssetDirectory* dir)
{
	std::string extension;
	// find the last occurrence of '.'
	size_t pos = file_path.find_last_of(".");
	// make sure the poisition is valid
	if (pos != file_path.length())
		extension = file_path.substr(pos + 1);
	else
		LOG("Coud not find . in the droped file path");

	//DevIL
	if (App->input->IsImageFormat(extension))
	{
		std::string name(file_path.substr(file_path.find_last_of("\\") + 1));
		TextureImportConfiguration import_config;
		TextureLoadConfiguration load_config;
		App->import_manager->Import(file_path, RT_TEXTURE, &import_config, &load_config, dir);
	}
	else if (extension == "fbx" || extension == "FBX")
	{
		SceneImportConfiguration* import_config = new SceneImportConfiguration;
		App->import_manager->Import(file_path, RT_PREFAB, import_config, nullptr, dir);
	}
	else if (extension == ".mm")
	{
		/*size_t pos = full_path.find_last_of("\\");
		size_t count = full_path.find_last_of(".") - pos;
		std::string file_name(full_path.substr(pos + 1, count - 1));*/
		//TODO should check path to se if in assets
		//App->import_manager->Load(file_name);
	}
	else
		LOG("Unknown file type");
}
