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

bool ImportManager::LoadMaterial(Material * to_load, const MaterialLoadConfiguration * load_config)
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	return material_importer->Load(to_load, App->resource_manager->GetNewMaterialPriority(), load_config);
}

bool ImportManager::LoadTexture(Texture * to_load, const TextureLoadConfiguration * load_config)
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}

	return texture_importer->Load(to_load, load_config);
}

bool ImportManager::LoadMesh(Mesh * to_load, const MeshLoadConfiguration * load_config)
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

bool ImportManager::LoadPrefab(Prefab * to_load, const PrefabLoadConfiguration * load_config)
{
	if (to_load->IsLoaded() == true)
	{
		LOG("Allready loaded this resource");
		return false;
	}
	return prefab_importer->Load(to_load, load_config);
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
	texture_importer->GenerateImage(path, &i_config, &l_config, ret, width, height);
	return ret;
}

const UID ImportManager::Import(const std::string & path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config) const
{
	if (!App->file_system->CopyToAssets(path))
	{
		LOG("Could not copy %s to assets", path.c_str());
		return UID();
	}

	UID uid;

	std::string file_name_ext(path.substr(path.find_last_of("\\") + 1));
	std::string file_name_no_ext(file_name_ext.substr(0, file_name_ext.find_last_of(".")));

	switch (type)
	{
	case RT_TEXTURE:
		uid = texture_importer->Import(file_name_ext, (TextureImportConfiguration*)import_config);
		if (uid.IsNull())
		{
			LOG("Could not import texture from %s corectlly", path.c_str());
			return UID();
		}
		break;
	case RT_PREFAB:
		uid = ImportScene(path, (SceneImportConfiguration*)import_config);
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

	if(type != RT_PREFAB)
		App->resource_manager->AddAsset(file_name_no_ext, uid, type, import_config, load_config);

	return uid;
}

bool ImportManager::MetaSave(const std::string & file, const UID & resource_id, RESOURCE_TYPE type, const ImportConfiguration * import_config, const LoadConfiguration * load_config) const
{
	unsigned int buff_size = file.size() + 1;
	buff_size += SIZE_OF_UID;
	buff_size += sizeof(type);
	buff_size += import_config->GetMetaSize();

	if (type != RT_PREFAB) //scenes have no load_config
		buff_size += load_config->GetMetaSize();

	char* buffer = new char[buff_size];
	char* iterator = buffer;

	memcpy(iterator, file.c_str(), file.length() + 1);
	iterator += file.length() + 1;

	memcpy(iterator, &resource_id, SIZE_OF_UID);
	iterator += SIZE_OF_UID;

	memcpy(iterator, &type, sizeof(type));
	iterator += sizeof(type);

	import_config->MetaSave(&iterator);

	if (type != RT_PREFAB) //scenes have no load_config
		load_config->MetaSave(&iterator);

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
			import_config->MetaLoad(&iterator);
			load_config = new TextureLoadConfiguration;
			load_config->MetaLoad(&iterator);
			new_asset = new TextureAsset(new_resource, import_config, load_config);
			return new_asset;
		case RT_PREFAB:
			//new_resource = new Scene(name, uid); TODO
			break;
		}
	}
	
	LOG("Metafile %s could not be loaded correctly", file.c_str());
	return nullptr;
}

const UID ImportManager::ImportScene(const std::string & path, const SceneImportConfiguration* import_config) const
{
	UID prefab_uid;

	/*std::string assets_path(App->file_system->GetAssets());
	path += "\\";
	path += file;*/

	std::string scene_name(GetImportFileNameNoExtension());
	std::string scene_dir(path.substr(0, path.find_last_of("\\") + 1)); // +1 to include \\

	Assimp::Importer* importer = new Assimp::Importer;

	//Config
	unsigned int flags =  aiProcess_ValidateDataStructure | aiProcess_ImproveCacheLocality | aiProcess_FindInvalidData | aiProcess_FindInstances;
	//aiProcess_TransformUVCoords  as long as there are no uv transforms
	//anim: aiProcess_LimitBoneWeights & aiProcess_SplitByBoneCount & aiProcess_Debone 
	//aiProcess_FixInfacingNormals result might not always be correct

	uint remove_components = 0;

	if (import_config->mesh_import_config->load_colors == false)
		remove_components |= aiComponent_COLORS;
	if (import_config->mesh_import_config->load_tangents == false)
		remove_components |= aiComponent_TANGENTS_AND_BITANGENTS;
	if (import_config->mesh_import_config->load_normals == false)
		remove_components |= aiComponent_NORMALS;
	else
		flags |= aiProcess_GenUVCoords | aiProcess_FixInfacingNormals;

	if (import_config->mesh_import_config->load_uvs == false)
		remove_components |= aiComponent_TEXCOORDS;
	else
		flags |= aiProcess_GenUVCoords | aiProcess_TransformUVCoords;

	if (import_config->mesh_import_config->load_bone_weights == false)
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
		flags |= aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_OptimizeMeshes;

	if (import_config->include_materials == false)
		remove_components |= aiComponent_MATERIALS;
	else
		flags |= aiProcess_RemoveRedundantMaterials;

	if (remove_components != 0)
	{
		flags |= aiProcess_RemoveComponent;
		importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, remove_components);
	}
	if (import_config->mesh_import_config->gen_normals)
		flags |= aiProcess_GenNormals;
	if (import_config->mesh_import_config->gen_smooth_normals)
		flags |= aiProcess_GenSmoothNormals;
	if (import_config->prefab_import_config->split_large_meshes)
		flags |= aiProcess_SplitLargeMeshes;
	if (import_config->prefab_import_config->pre_transform)
		flags |= aiProcess_PreTransformVertices;
	if (import_config->prefab_import_config->split_large_meshes)
		flags |= aiProcess_SplitLargeMeshes;
	if (import_config->prefab_import_config->optimize_graph)
		flags |= aiProcess_OptimizeGraph;
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

		std::vector<UID> scene_materials;
		std::vector<UID> scene_meshes;

		if (scene->HasMaterials())
		{
			scene_materials.reserve(scene->mNumMaterials);
			material_loads = new bool[scene->mNumMaterials];

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
				{
					LOG("Importing Material %i", i);

					char ptr[255];
					sprintf(ptr, "Material_%s_%i", scene_name.c_str(), i);
					std::string material_name(ptr);

					UID uid(material_importer->Import(scene_dir, scene->mMaterials[i], import_config->material_import_config));

					if (uid.IsNull() == false)
					{
						material_loads[i] = true;
						scene_materials.push_back(uid);
						App->resource_manager->AddAsset(material_name, uid, RT_MATERIAL, new MaterialImportConfiguration(*import_config->material_import_config), new MaterialLoadConfiguration(*import_config->material_load_config));
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

				UID uid(mesh_importer->Import(scene->mMeshes[i], import_config->mesh_import_config));

				mesh_loads[i] = true;
				scene_meshes.push_back(uid);

				if (uid.IsNull() == false)
				{
					mesh_loads[i] = true;
					scene_meshes.push_back(uid);
					App->resource_manager->AddAsset(mesh_name, uid, RT_MESH, new MeshImportConfiguration(*import_config->mesh_import_config), new MeshLoadConfiguration(*import_config->mesh_load_config));
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
			prefab_uid = prefab_importer->Import(scene, scene_materials, material_loads, scene_meshes, mesh_loads);

			if (prefab_uid.IsNull() == false)
				App->resource_manager->AddAsset(scene_name, prefab_uid, RT_PREFAB, import_config->prefab_import_config, import_config->prefab_load_config);
			else
			{
				LOG("Scene could not be loaded as prefab");
				prefab_uid = UID();
			}
		}

		delete[] material_loads;
		delete[] mesh_loads;
		delete importer;
	}
	else	
		LOG("Error %s loading scene %s", aiGetErrorString(), path.c_str());
		
	return prefab_uid;
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

const UID ImportManager::ImportClient::Import(const ImportManager * importer, const std::string & path, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config)
{
	return importer->Import(path, type, import_config, load_config);
}
