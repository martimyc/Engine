#include "glew/include/GL/glew.h"
#include "imgui\imgui.h"

#include "Dirent\dirent.h"

#include "UID.h"

//Resources
#include "Resource.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "PreFab.h"
#include "Skeleton.h"
#include "Animation.h"

//Assets
#include "Asset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "SkeletonAsset.h"
#include "AnimationAsset.h"

#include "ImportManager.h"
#include "ResourceManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "AssetDirectory.h"

GLuint AssetDirectory::icon = 0;

AssetDirectory::AssetDirectory(const std::string& path): path(path)
{}

AssetDirectory::AssetDirectory(const AssetDirectory& copy) : path(copy.path), last_modified(copy.last_modified), assets(copy.assets), directories(copy.directories)
{
}

AssetDirectory::~AssetDirectory()
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		delete *it;
	assets.clear();

	for (std::vector<AssetDirectory*>::iterator it = directories.begin(); it != directories.end(); ++it)
		delete *it;
	directories.clear();
}

void AssetDirectory::SetImage(GLuint id)
{
	icon = id;
}

const std::string & AssetDirectory::GetPath() const
{
	return path;
}

std::string AssetDirectory::GetName() const
{
	return path.substr(path.find_last_of("\\") + 1);
}

time_t AssetDirectory::GetTimeStamp() const
{
	return last_modified;
}

void AssetDirectory::SetTimeStamp(time_t new_mod_time)
{
	last_modified = new_mod_time;
}

void AssetDirectory::AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config)
{
	Resource* new_resource = nullptr;
	Asset* new_asset = nullptr;

	switch (type)
	{
	case RT_TEXTURE:
		new_resource = new Texture(name, uid);
		App->import_manager->LoadTexture((Texture*)new_resource,(TextureLoadConfiguration*)load_config); //textures will always be loaded to have image show in assets
		new_asset = new TextureAsset(new_resource, new TextureImportConfiguration(*(TextureImportConfiguration*)import_config), new TextureLoadConfiguration(*(TextureLoadConfiguration*)load_config));
		break;
	case RT_PREFAB:
		new_resource = new Prefab(name, uid);
		new_asset = new PrefabAsset(new_resource, new PrefabImportConfiguration(*(PrefabImportConfiguration*)import_config), new PrefabLoadConfiguration(*(PrefabLoadConfiguration*)load_config));
		break;
	case RT_MESH:
		new_resource = new Mesh(name, uid);
		new_asset = new MeshAsset(new_resource, new MeshImportConfiguration(*(MeshImportConfiguration*)import_config), new MeshLoadConfiguration(*(MeshLoadConfiguration*)load_config));
		break;
	case RT_MATERIAL:
		new_resource = new Material(name, uid);
		new_asset = new MaterialAsset(new_resource, new MaterialImportConfiguration(*(MaterialImportConfiguration*)import_config), new MaterialLoadConfiguration(*(MaterialLoadConfiguration*)load_config));
		break;
	case RT_SKELETON:
		new_resource = new Skeleton(name, uid);
		new_asset = new SkeletonAsset((Skeleton*)new_resource, new SkeletonImportConfiguration(*(SkeletonImportConfiguration*)import_config), new SkeletonLoadConfiguration(*(SkeletonLoadConfiguration*)load_config));
		break;
	case RT_ANIMATION:
		new_resource = new Animation(name, uid);
		new_asset = new AnimationAsset((Animation*)new_resource, new AnimationImportConfiguration(*(AnimationImportConfiguration*)import_config), new AnimationLoadConfiguration(*(AnimationLoadConfiguration*)load_config));
		break;
	default:
		LOG("Non standard import type");
		break;
	}
	assets.push_back(new_asset);
}

void AssetDirectory::AddAsset(Asset * new_asset)
{
	assets.push_back(new_asset);
}

void AssetDirectory::AddDir(AssetDirectory * new_directory)
{
	directories.push_back(new_directory);
}

void AssetDirectory::RemoveAsset(Asset * to_remove)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (to_remove == *it)
		{
			assets.erase(it);
			break;
		}
}

void AssetDirectory::RemoveDir(AssetDirectory * to_remove)
{
	for (std::vector<AssetDirectory*>::iterator it = directories.begin(); it != directories.end(); ++it)
		if (to_remove == *it)
			directories.erase(it);
}

void AssetDirectory::Inspector(unsigned int& selected)
{
	ImVec2 image_size(50,50);
	int i = 0;

	ImVec2 start_pos = ImGui::GetCursorPos();
	float start_width = ImGui::GetColumnWidth();

	for (std::vector<AssetDirectory*>::iterator it = directories.begin(); it != directories.end(); ++it)
	{
		if(ImGui::Asset((*it)->GetName().c_str(), image_size, i == selected ? true : false,(void*) icon))
			App->resource_manager->SetCurrentDir(*it);

		if (ImGui::IsItemClicked())
			selected = i;

		i++;
		
		if((ImGui::GetItemRectMax().x > start_pos.x + ImGui::GetColumnWidth() - 150) == false) //TODO fix with dock size
			ImGui::SameLine();
	}

	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if (ImGui::Asset((*it)->GetName().c_str(), image_size, i == selected ? true : false, (void*)(*it)->GetImage()))
			App->resource_manager->LoadToScene(*it);

		if (ImGui::IsItemClicked())
			selected = i;

		i++;
		
		if ((ImGui::GetItemRectMax().x > start_pos.x + ImGui::GetColumnWidth() - 150) == false)//TODO fix with dock size
			ImGui::SameLine();
	}
}

bool AssetDirectory::Hirarchy(AssetDirectory ** focused)
{
	bool ret = false;
	bool has_childs = directories.size() != 0;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((*focused == this) ? ImGuiTreeNodeFlags_Selected : 0);

	if (!has_childs)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
		node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool node_open = ImGui::TreeNodeEx(this, node_flags, GetName().c_str()); //TODO try with TreeNode*

	if (ImGui::IsItemClicked() && ret == false)
	{
		*focused = this;
		ret = true;
	}

	if (node_open)
	{
		for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
			if ((*it)->Hirarchy(focused))
				ret = true;

		if (has_childs)
			ImGui::TreePop();
	}

	return ret;
}


Material * AssetDirectory::UseMaterial(const UID & id, const GameObject * go) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Material* material = (Material*)(*it)->GetResource();

			if (material->IsLoaded() == false)
				if (App->import_manager->LoadMaterial(material, (MaterialLoadConfiguration*)(*it)->GetLoadConfig()) == false)
					LOG("Could not load source for material %s correctly", (*it)->GetName().c_str());

			((MaterialAsset*)(*it))->AddInstance(go);

			return material;
		}
	return nullptr;
}

Texture * AssetDirectory::UseTexture(const UID & id, const Material * material) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Texture* texture = (Texture*)(*it)->GetResource();

			((TextureAsset*)(*it))->AddInstance(material);

			return texture;
		}
	return nullptr;
}

Mesh * AssetDirectory::UseMesh(const UID & id, const GameObject * go) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Mesh* mesh = (Mesh*)(*it)->GetResource();

			if (mesh->IsLoaded() == false)
				if (App->import_manager->LoadMesh(mesh, (MeshLoadConfiguration*)(*it)->GetLoadConfig()) == false)
					LOG("Could not load source for mesh %s correctly", (*it)->GetName().c_str());

			((MeshAsset*)(*it))->AddInstance(go);
			return mesh;
		}
	return nullptr;
}

Prefab * AssetDirectory::UsePrefab(const UID & id, const GameObject * go) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Prefab* prefab = (Prefab*)(*it)->GetResource();

			if (prefab->IsLoaded() == false)
				if (App->import_manager->LoadPrefab(prefab, (PrefabLoadConfiguration*)(*it)->GetLoadConfig()) == false)
					LOG("Could not load source for prefab %s correctly", (*it)->GetName().c_str());

			((PrefabAsset*)(*it))->AddInstance(go);
			return prefab;
		}
	return nullptr;
}

Skeleton * AssetDirectory::UseSkeleton(const UID & id, const GameObject * go) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Skeleton* skeleton = (Skeleton*)(*it)->GetResource();

			if (skeleton->IsLoaded() == false)
				if (App->import_manager->LoadSkeleton(skeleton, (SkeletonLoadConfiguration*)(*it)->GetLoadConfig()) == false)
					LOG("Could not load source for skeleton %s correctly", (*it)->GetName().c_str());

			((SkeletonAsset*)(*it))->AddInstance(go);
			return skeleton;
		}
	return nullptr;
}

Animation * AssetDirectory::UseAnimation(const UID & id, const GameObject * go) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Animation* animation = (Animation*)(*it)->GetResource();

			if (animation->IsLoaded() == false)
				if (App->import_manager->LoadAnimation(animation, (AnimationLoadConfiguration*)(*it)->GetLoadConfig()) == false)
					LOG("Could not load source for animation %s correctly", (*it)->GetName().c_str());

			((AnimationAsset*)(*it))->AddInstance(go);
			return animation;
		}
	return nullptr;
}

void AssetDirectory::StopUsingMaterial(Material * material, const GameObject * go) 
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (material == (Material*)*it)
			((MaterialAsset*)(*it))->DeleteInstance(go);
}

void AssetDirectory::StopUsingTexture(Texture * text, const Material * material)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (text == (Texture*)*it)
			((TextureAsset*)(*it))->DeleteInstance(material);
}

void AssetDirectory::StopUsingMesh(Mesh * mesh, const GameObject * go)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (mesh == (Mesh*)*it)
			((MeshAsset*)(*it))->DeleteInstance(go);
}

void AssetDirectory::StopUsingPrefab(Prefab * prefab, const GameObject * go)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (prefab == (Prefab*)*it)
			((PrefabAsset*)(*it))->DeleteInstance(go);
}

void AssetDirectory::StopUsingSkeleton(Skeleton * skeleton, const GameObject * go)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (skeleton == (Skeleton*)*it)
			((SkeletonAsset*)(*it))->DeleteInstance(go);
}

void AssetDirectory::StopUsingAnimation(Animation * anim, const GameObject * go)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (anim == (Animation*)*it)
			((AnimationAsset*)(*it))->DeleteInstance(go);
}

void AssetDirectory::Update()
{
	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		/* Create assets for all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			std::string file(ent->d_name);

			if (ent->d_type == DT_REG)
			{
				if (file == "." || file == ".." || file == "Thumbs.db")
					continue;
				if (App->file_system->IsMeta(file))
				{
					std::string real_file(path);
					real_file += "\\" + file.substr(0, file.find_last_of("."));
					time_t file_mod_time = App->file_system->GetTimeStamp(real_file);

					App->import_manager->ReImportWithMeta(file_mod_time,path + "\\" + file, this);
				}
				else if (App->file_system->HasMeta(file, path) == false)
				{
					std::string file_path(path + "\\" + file);
					ImportManager::ImportClient::StraightImport(file_path, this);
				}
			}
		}
		closedir(dir);
	}
	else
		LOG("Directory %s could not be opened", path.c_str());

	for (std::vector<AssetDirectory*>::iterator it = directories.begin(); it != directories.end(); ++it)
	{
		time_t mod = App->file_system->GetTimeStamp((*it)->GetPath());
		if (mod != (*it)->last_modified)
		{
			(*it)->SetTimeStamp(mod);
			(*it)->Update();
		}
	}

}

void AssetDirectory::DeleteAsset(const UID & uid)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if (uid == (*it)->GetUID())
		{
			delete *it;
			assets.erase(it);
			break;
		}
}

Texture * AssetDirectory::GetTexture(const UID & uid) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Texture*)(*it)->GetResource();

	Texture* ret = nullptr;
	for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
	{
		ret = (*it)->GetTexture(uid);
		if (ret != nullptr)
			break;
	}
	return ret;
}

Material * AssetDirectory::GetMaterial(const UID & uid) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Material*)(*it)->GetResource();

	Material* ret = nullptr;
	for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
	{
		ret = (*it)->GetMaterial(uid);
		if (ret != nullptr)
			break;
	}
	return ret;
}

Mesh * AssetDirectory::GetMesh(const UID & uid) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Mesh*)(*it)->GetResource();

	Mesh* ret = nullptr;
	for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
	{
		ret = (*it)->GetMesh(uid);
		if (ret != nullptr)
			break;
	}
	return ret;
}

Prefab * AssetDirectory::GetPrefab(const UID & uid) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Prefab*)(*it)->GetResource();

	Prefab* ret = nullptr;
	for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
	{
		ret = (*it)->GetPrefab(uid);
		if (ret != nullptr)
			break;
	}
	return ret;
}


bool AssetDirectory::Exsists(const UID & uid) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return true;

	bool ret = false;
	for (std::vector<AssetDirectory*>::const_iterator it = directories.begin(); it != directories.end(); ++it)
	{
		ret = (*it)->Exsists(uid);
		if (ret == true)
			break;
	}
	return ret;
}

void AssetDirectory::LoadPresentation()
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetName() == "Street")
			App->resource_manager->LoadToScene(*it);

	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetName() == "AA")
			App->resource_manager->LoadToScene(*it);

	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetName() == "Take 001")
			App->resource_manager->LoadToScene(*it);
}
