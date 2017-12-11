#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"

#include "UID.h"

//Resources
#include "Resource.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "PreFab.h"
#include "Animation.h"
#include "Skeleton.h"

//Assets
#include "Asset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "AnimationAsset.h"
#include "SkeletonAsset.h"

#include "AssetDirectory.h"
#include "GameObject.h"

//Components
#include "AppliedMaterial.h"

//Modules
#include "FileSystem.h"
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
	return root_dir->GetTexture(uid);
}

Material * ResourceManager::GetMaterial(const UID & uid)
{
	return root_dir->GetMaterial(uid);
}

Mesh * ResourceManager::GetMesh(const UID & uid)
{
	return root_dir->GetMesh(uid);
}

Prefab * ResourceManager::GetPrefab(const UID & uid)
{
	return root_dir->GetPrefab(uid);
}

Material * ResourceManager::CreateEmptyMaterial(const char* name)
{
	Material* new_material = nullptr;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Material %i", num_materials);
		new_material = new Material(new_name, new MaterialSource(++material_priority));
		current_dir->AddAsset(new MaterialAsset(new_material, nullptr, nullptr));
	}
	else
	{
		new_material = new Material(name, new MaterialSource(++material_priority));
		current_dir->AddAsset(new MaterialAsset(new_material, nullptr, nullptr));
	}

	return new_material;
}

void ResourceManager::LoadToScene(Asset* asset)
{
	switch (asset->GetType())
	{
	case RT_TEXTURE:
		if (App->scene_manager->GetFocused()->HasMeshFilter())
		{
			//TODO fix
			if (App->scene_manager->GetFocused()->GetMaterial() == nullptr)
			{
				Material* material = CreateEmptyMaterial();
				Texture* texture = current_dir->UseTexture(asset->GetUID(), material);
				material->AddTexture(texture, TT_DIFFUSE);
				App->scene_manager->GetFocused()->AddComponent(new AppliedMaterial(material));
			}
			else
			{
				AppliedMaterial* material = App->scene_manager->GetFocused()->GetAppliedMaterial();
				Texture* texture = current_dir->UseTexture(asset->GetUID(), material->GetMaterial());
				material->AddTexture(texture);
			}
		}
		else
			LOG("No mesh to apply texture to");
		break;
	case RT_PREFAB:
		App->scene_manager->AddPrefabAsNewGameObjects(current_dir->UsePrefab(asset->GetUID(), App->scene_manager->GetFocused())->GetRoot());
		break;
	case RT_MESH:
		App->scene_manager->GetFocused()->ChangeMesh(current_dir->UseMesh(asset->GetUID(), App->scene_manager->GetFocused()));
		break;
	case RT_MATERIAL:
		App->scene_manager->GetFocused()->ChangeMaterial(current_dir->UseMaterial(asset->GetUID(), App->scene_manager->GetFocused()));
		break;
	}
}

void ResourceManager::SetRootDir(AssetDirectory * dir)
{
	root_dir = dir;
	current_dir = dir;
}

void ResourceManager::SetCurrentDir(AssetDirectory * dir)
{
	current_dir = dir;
}

bool ResourceManager::CopyFileToCurrentDir(const std::string & path) const
{
	return App->file_system->CopyTo(path, current_dir->GetPath());
}

const std::string & ResourceManager::GetCurrentDirPath() const
{
	return current_dir->GetPath();
}

void ResourceManager::UpdateAssets()
{
	time_t mod = App->file_system->GetTimeStamp(root_dir->GetPath());
	if (mod != root_dir->GetTimeStamp())
	{
		root_dir->SetTimeStamp(mod);
		root_dir->Update();
	}
}

bool ResourceManager::Start()
{
	AssetDirectory* assets = App->file_system->GenerateAssets(App->file_system->GetAssets());
	if (assets != nullptr)
	{
		root_dir = assets;
		current_dir = root_dir;
	}
	else
		LOG("Could not generate assets");

	debug_textures = LoadCheckers();
	selected = 0;
	LoadButtons();
	LoadIcons();

	return true;
}

UPDATE_STATUS ResourceManager::Update(float dt)
{
	ImGui::ShowTestWindow();
	if (App->BeginDockWindow("Assets"))
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 200);
		root_dir->Hirarchy(&current_dir);
		ImGui::NextColumn();
		current_dir->Inspector(selected);
	}
	App->EndDockWindow();

	return UPDATE_CONTINUE;
}

bool ResourceManager::CleanUp()
{
	delete root_dir;

	return true;
}

void ResourceManager::AddAsset(const std::string & name, const UID & uid, RESOURCE_TYPE type, const ImportConfiguration * import_config, const LoadConfiguration * load_config)
{
	current_dir->AddAsset( name, uid, type, import_config, load_config);
	switch (type)
	{
	case RT_TEXTURE:num_textures++;	break;
	case RT_MESH: num_meshes++;	break;
	case RT_MATERIAL: num_materials++; break;
	case RT_PREFAB: num_prefabs++;break;
	}
}

void ResourceManager::DeleteAsset(Asset * to_delete)
{
	current_dir->RemoveAsset(to_delete);
	switch (to_delete->GetType())
	{
	case RT_TEXTURE:num_textures--;	break;
	case RT_MESH: num_meshes--;	break;
	case RT_MATERIAL: num_materials--; break;
	case RT_PREFAB: num_prefabs--; break;
	}
}

bool ResourceManager::Exsists(const UID & id) const
{
	return root_dir->Exsists(id);
}

Material * ResourceManager::UseMaterial(const UID & id, const GameObject * go) const
{
	return current_dir->UseMaterial(id, go);
}

Texture * ResourceManager::UseTexture(const UID & id, const Material * material) const
{
	return current_dir->UseTexture(id, material);
}

Mesh * ResourceManager::UseMesh(const UID & id, const GameObject * go) const
{
	return current_dir->UseMesh(id, go);
}

Prefab * ResourceManager::UsePrefab(const UID & id, const GameObject * go) const
{
	return current_dir->UsePrefab(id, go);
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
	root_dir->AddAsset(new TextureAsset(new_texture, nullptr, nullptr));
	num_textures++;

	return new_texture;
}

void ResourceManager::LoadButtons()
{
	buttons.push_back(new Button(BT_PLAY, App->import_manager->GenerateButtonImage("..\\Icons\\Buttons\\PlayButton.png"), 25, 25));
	buttons.push_back(new Button(BT_PAUSE, App->import_manager->GenerateButtonImage("..\\Icons\\Buttons\\PauseButton.png"), 25, 25));
	buttons.push_back(new Button(BT_PLAY_ONE_FRAME, App->import_manager->GenerateButtonImage("..\\Icons\\Buttons\\PlayOneFrameButton.png"), 25, 25));
}

void ResourceManager::LoadIcons()
{
	AssetDirectory::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\FolderIcon.png"));
	MeshAsset::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\Mesh.png"));
	MaterialAsset::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\Material.png"));
	PrefabAsset::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\Scene.png"));
	AnimationAsset::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\Animation.png"));
	SkeletonAsset::SetImage(App->import_manager->GenerateButtonImage("..\\Icons\\Rigg.png"));
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
