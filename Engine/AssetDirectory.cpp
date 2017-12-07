#include "glew/include/GL/glew.h"
#include "imgui\imgui.h"

#include "UID.h"

//Resources
#include "Resource.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "PreFab.h"

//Assets
#include "Asset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"

#include "ImportManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "AssetDirectory.h"

GLuint AssetDirectory::icon = 0;

AssetDirectory::AssetDirectory(const std::string& path): path(path)
{}

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
	default:
		LOG("Non standard import type");
		break;
	}
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

	ImGui::BeginGroup();

	for (std::vector<AssetDirectory*>::iterator it = directories.begin(); it != directories.end(); ++it)
	{
		if(ImGui::Asset((*it)->GetName().c_str(), image_size, i == selected ? true : false,(void*) icon))
			App->resource_manager->SetCurrentDir(*it);

		if (ImGui::IsItemClicked())
			selected = i;

		i++;

		if(ImGui::GetCursorPos().x + image_size.x > ImGui::GetWindowContentRegionMax().x == false)
			ImGui::SameLine();
	}

	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if (ImGui::Asset((*it)->GetName().c_str(), image_size, i == selected ? true : false, (void*)(*it)->GetImage()))
			App->resource_manager->LoadToScene(*it);

		if (ImGui::IsItemClicked())
			selected = i;

		i++;
		
		if (ImGui::GetCursorPos().x + image_size.x > ImGui::GetWindowContentRegionMax().x == false)
			ImGui::SameLine();
	}

	ImGui::EndGroup();
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
