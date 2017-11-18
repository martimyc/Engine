#include "glew\include\GL\glew.h"
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
#include "GameObject.h"

//Components
#include "AppliedMaterial.h"

//Modules
#include "ImportManager.h"
#include "SceneManager.h"
#include "TimeManager.h"
#include "Application.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

ResourceManager::~ResourceManager()
{}

Texture * ResourceManager::GetTexture(const UID & uid)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Texture*)*it;

	return nullptr;
}

Material * ResourceManager::GetMaterial(const UID & uid)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Material*)*it;

	return nullptr;
}

Mesh * ResourceManager::GetMesh(const UID & uid)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Mesh*)*it;

	return nullptr;
}

Prefab * ResourceManager::GetPrefab(const UID & uid)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetUID() == uid)
			return (Prefab*)*it;

	return nullptr;
}

Material * ResourceManager::CreateEmptyMaterial(const char* name)
{
	Material* new_material = nullptr;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Material %i", num_materials);
		new_material = new Material(new_name, new MaterialSource(++material_priority));
		assets.push_back(new MaterialAsset(new_material, nullptr, nullptr));
	}
	else
	{
		new_material = new Material(name, new MaterialSource(++material_priority));
		assets.push_back(new MaterialAsset(new_material, nullptr, nullptr));
	}

	return new_material;
}

void ResourceManager::LoadToScene()
{
	switch (selected_asset->GetType())
	{
	case RT_TEXTURE:
		if (App->scene_manager->GetFocused()->GetMaterial() == nullptr)
		{
			Material* material = CreateEmptyMaterial();
			Texture* texture = UseTexture(selected_asset->GetUID(), material);
			material->AddTexture(texture, TT_DIFFUSE);
			App->scene_manager->GetFocused()->ChangeMaterial(material);
		}
		else
		{
			UID uid(App->scene_manager->GetFocused()->GetMaterial()->GetUID());
			Material* material = GetMaterial(uid);
			Texture* texture = UseTexture(selected_asset->GetUID(), material);
			material->AddTexture(texture, TT_DIFFUSE);
		}
		break;
	case RT_PREFAB:
		App->scene_manager->GetFocused()->AddChild(UsePrefab(selected_asset->GetUID(), App->scene_manager->GetFocused())->GetRoot());
		break;
	case RT_MESH:
		App->scene_manager->GetFocused()->ChangeMesh(UseMesh(selected_asset->GetUID(), App->scene_manager->GetFocused()));
		break;
	case RT_MATERIAL:
		App->scene_manager->GetFocused()->ChangeMaterial(UseMaterial(selected_asset->GetUID(), App->scene_manager->GetFocused()));
		break;
	}
}

bool ResourceManager::Init()
{
	debug_textures = LoadCheckers();
	selected_asset = assets[0];
	LoadButtons();

	return true;
}

UPDATE_STATUS ResourceManager::Update(float dt)
{
	if (ImGui::Begin("Assets"))
	{
		for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected_asset == *it) ? ImGuiTreeNodeFlags_Selected : 0);

			bool node_open = ImGui::TreeNodeEx(*it, node_flags, (*it)->GetName().c_str()); //TODO try with TreeNode*

			if (ImGui::IsItemClicked())
				selected_asset = *it;

			if (node_open)
			{
				std::string uid((*it)->GetUID().GetAsName());
				ImGui::Text("UID: %s", uid.c_str());
				ImGui::TreePop();
			}
		}

		if (ImGui::Button("Load To Scene"))
			LoadToScene();
	}
	ImGui::End();
	return UPDATE_CONTINUE;
}

bool ResourceManager::CleanUp()
{

	for (std::vector<Button*>::iterator it = buttons.begin(); it != buttons.end(); ++it)
		DELETE_PTR(*it);
	buttons.clear();
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		DELETE_PTR(*it);
	assets.clear();

	return true;
}

void ResourceManager::AddAsset(const std::string& name, const UID& uid, RESOURCE_TYPE type, const ImportConfiguration* import_config, const LoadConfiguration* load_config)
{
	Resource* new_resource = nullptr;
	Asset* new_asset = nullptr;

	switch (type)
	{
	case RT_TEXTURE:
		new_resource = new Texture(name, uid);
		App->import_manager->LoadTexture((Texture*)new_resource, (TextureLoadConfiguration*)load_config); //textures will always be loaded to have image show in assets
		new_asset = new TextureAsset(new_resource, import_config, load_config);
		num_textures++;
		break;
	case RT_PREFAB:
		new_resource = new Prefab(name, uid);
		new_asset = new PrefabAsset(new_resource, import_config, load_config);
		num_prefabs++;
		break;
	case RT_MESH:
		new_resource = new Mesh(name, uid);
		new_asset = new MeshAsset(new_resource, import_config, load_config);
		num_meshes++;
		break;
	case RT_MATERIAL:
		new_resource = new Material(name, uid);
		new_asset = new MaterialAsset(new_resource, import_config, load_config);
		num_materials++;
		break;
	default:
		LOG("Non standard import type");
		break;
	}
	assets.push_back(new_asset);
}

void ResourceManager::DeleteAsset(Asset * to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (to_delete == *it)
			delete *it;
}

bool ResourceManager::Exsists(const UID & id) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
			return true;
	return false;
}

Material * ResourceManager::UseMaterial(const UID & id, const GameObject * go) const
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

Texture * ResourceManager::UseTexture(const UID & id, const Material * material) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (id == (*it)->GetUID())
		{
			Texture* texture = (Texture*) (*it)->GetResource();

			((TextureAsset*)(*it))->AddInstance(material);

			return texture;
		}
	return nullptr;
}

Mesh * ResourceManager::UseMesh(const UID & id, const GameObject * go) const
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

Prefab * ResourceManager::UsePrefab(const UID & id, const GameObject * go) const
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

Texture* ResourceManager::LoadCheckers()
{
	TextureSource* source = new TextureSource;

	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	//Load Texture to VRAM
	GLuint checkers_text_id;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkers_text_id);
	glBindTexture(GL_TEXTURE_2D, checkers_text_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Anysotropy
	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	source->SetTextureID(checkers_text_id);
	source->SetDimensions(CHECKERS_WIDTH, CHECKERS_HEIGHT);

	Texture* new_texture = new Texture("Checkers", source);
	assets.push_back(new TextureAsset(new_texture, nullptr, nullptr));
	num_textures++;

	return new_texture;
}

void ResourceManager::LoadButtons()
{
	buttons.push_back(new Button(BT_PLAY, App->import_manager->GenerateButtonImage("Buttons\\PlayButton.png"), 50, 50));
	buttons.push_back(new Button(BT_PAUSE, App->import_manager->GenerateButtonImage("Buttons\\PauseButton.png"), 50, 50));
	buttons.push_back(new Button(BT_PLAY_ONE_FRAME, App->import_manager->GenerateButtonImage("Buttons\\PlayOneFrameButton.png"), 50, 50));
}

unsigned int ResourceManager::GetNewMaterialPriority()
{
	return num_materials + 1; //meshes without material have priority 0
}

void ResourceManager::DebugTextures() const
{
	if (debug_textures)
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, debug_texture->GetTextureID());

		float hsize = 6.0f; // Vertical size of the quad
		float vsize = 4.0f; // Vertical size of the quad

							// Draw our texture
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);

		// Top right
		glTexCoord2f(1.0, 0.0);
		glVertex3f(hsize, -vsize, 0.0f);

		// Bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex3f(hsize, vsize, 0.0f);

		// Bottom left
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-hsize, vsize, 0.0f);

		// Top left
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-hsize, -vsize, 0.0f);

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}

void ResourceManager::CreateButtons() const
{
	for (std::vector<Button*>::const_iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		if (ImGui::ImageButton((void*)(*it)->id, ImVec2((*it)->width, (*it)->height)))
		{
			switch ((*it)->type)
			{
			case BT_PLAY:
				App->time_manager->PlayGame();
				break;

			case BT_PAUSE:
				App->time_manager->PauseGame();
				break;

			case BT_PLAY_ONE_FRAME:
				App->time_manager->DoOneUpdate();
				break;

			default:
				break;
			}
		}
		ImGui::SameLine();
	}
}
