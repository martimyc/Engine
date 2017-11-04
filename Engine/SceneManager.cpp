#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "MathGeoLib\src\Geometry\AABB.h"

//Components & Assets
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "AppliedMaterial.h"
#include "MeshFilter.h"
#include "Transform.h"

#include "KDTree.h"

//Modules
#include "Renderer3D.h"
#include "BasicGeometry.h"
#include "GameObject.h"
#include "ImportManager.h"
#include "Application.h"
#include "SceneManager.h"

SceneManager::SceneManager(const char * name, bool start_enabled) : Module(name, start_enabled), draw_mode(DM_NORMAL), wireframe(false), normals(false), polygons(true)
{}

SceneManager::~SceneManager()
{}

bool SceneManager::Init()
{
	go_kdtree = new KDTree();

	root = new GameObject(nullptr, "Root");
	focused = root;

	go_kdtree->AddGameObject(root);

	App->import_manager->LoadCheckers();

	return true;
}

bool SceneManager::CleanUp()
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		delete (*it);
	assets.clear();

	return true;
}

UPDATE_STATUS SceneManager::Configuration(float dt)
{
	BROFILER_CATEGORY("Scene Manager Configuration", Profiler::Color::BlanchedAlmond)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (App->BeginDockWindow("Draw Modes", &config_scene))
	{
		if (ImGui::Checkbox("Wireframe", &wireframe))
		{
			if (wireframe)
			{
				draw_mode = DM_WIREFRAME;
				polygons = false;
			}
			else
			{
				draw_mode = DM_NORMAL;
				polygons = true;
			}
		}

		if (ImGui::Checkbox("Normals", &normals))
		{
			if (normals)
				draw_mode = DM_NORMALS;
			else
			{
				if (wireframe)
					draw_mode = DM_WIREFRAME;
				else if (polygons)
					draw_mode = DM_NORMAL;
			}
		}

		if (ImGui::Checkbox("Polygon", &polygons))
		{
			if (polygons)
			{
				draw_mode = DM_NORMAL;
				wireframe = false;
			}
			else
			{
				draw_mode = DM_WIREFRAME;
				wireframe = true;
			}
		}
	}
	App->EndDockWindow();

	if (App->BeginDockWindow("Debug", &debug))
	{
		//All debug bools
		if (ImGui::Button("Debug Textures"))
			debug_textures = !debug_textures;

		if (debug_textures)
		{
			if (ImGui::InputInt("Texture to draw", &texture_to_draw))
			{
				if (texture_to_draw >= GetNumTextures())
				{
					LOG("Texture %i does not exist, binding Checkers", texture_to_draw);
					texture_to_draw = 0;
				}
				else if (texture_to_draw < 0)
					texture_to_draw = 0;
			}
		}
	}
	App->EndDockWindow();

	ImGui::ShowTestWindow();

	if(ImGui::Begin("Assets", &assets_enable))
	{
		for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		{
			//ImGui::ListBox((*it)->GetName().c_str(),);
			//(*it)
		}
		ImGui::End();
	}

	Hirarchy();

	return ret;
}
/*
UPDATE_STATUS SceneManager::MaterialsConfiguration(float dt)
{
	if(materials.size() > 0)
		if (ImGui::CollapsingHeader("Materials"))
		{
			if (ImGui::Button("Create material"))
				CreateMaterial();

			std::vector<int> materials_to_delete;
			bool node_open = false;

			ImGui::Text("Materials:");
			for (int i = 0; i < materials.size(); i++)
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected_material == i) ? ImGuiTreeNodeFlags_Selected : 0);

				node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, materials[i]->GetName().c_str());

				if (ImGui::IsItemClicked())
					selected_material = i;

				if(node_open)
				{
					materials[i]->LoneConfig();

					if ( focused != nullptr)
					{
						char apply[255];
						sprintf(apply, "Apply to %s mesh num:", focused->GetName().c_str());

						if (ImGui::Button(apply))
							if (focused != nullptr)
								focused->ChangeMaterial(materials[i]);
							else
								LOG("Can not add material, no focused game object");
					}

					if (ImGui::Button("Delete material"))
					{
						LOG("Deleting material");
						materials_to_delete.push_back(i);
					}
					ImGui::TreePop();
				}
			}

			for (std::vector<int>::const_iterator it = materials_to_delete.begin(); it != materials_to_delete.end(); ++it)
			{
				delete materials[*it];
				materials.erase(materials.begin() + (*it));
			}
		}

	return UPDATE_CONTINUE;
}

UPDATE_STATUS SceneManager::TexturesConfiguration(float dt)
{
	if (textures.size() > 0)
		if (ImGui::CollapsingHeader("Textures"))
		{
				

				

				std::vector<int> textures_to_delete;
				bool node_open = false;

				for (int i = 0; i < textures.size(); i++)
				{
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected_texture == i) ? ImGuiTreeNodeFlags_Selected : 0);

					node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, textures[i]->GetName().c_str());

					if (ImGui::IsItemClicked())
						selected_texture = i;

					if (node_open)
					{
						ImVec2 size(50, 50);
						ImVec2 uv0(0, 1);
						ImVec2 uv1(1, 0);

						ImGui::Image((void*)textures[i]->GetID(), size, uv0, uv1);
						ImGui::SameLine();
						ImGui::Text("Width: %i\nHeight: %i", textures[i]->GetWidth(), textures[i]->GetHeight());

						if (ImGui::Button("Delete"))
						{
							LOG("Deleting textures");
							textures_to_delete.push_back(i);
						}

						ImGui::TreePop();
					}
				}

				char add[255];
				sprintf(add, "Add %s to material", textures[selected_texture]->GetName().c_str());

				if (ImGui::Button(add))
					App->scene_manager->ApplyToMaterial(textures[selected_texture], current_material);
				ImGui::SameLine();

				if (ImGui::InputInt("", &current_material))
				{
					if (current_material < 0)
						current_material = 0;
					else if (current_material > App->scene_manager->NumMaterials())
					{
						LOG("Material %i does not exsist", current_material);
						current_material = App->scene_manager->NumMaterials();
					}
				}

				for (std::vector<int>::const_iterator it = textures_to_delete.begin(); it != textures_to_delete.end(); ++it)
				{
					delete textures[*it];
					textures.erase(textures.begin() + (*it));
				}
			}
	return UPDATE_CONTINUE;
}*/

UPDATE_STATUS SceneManager::Update(float dt)
{
	UpdateGameObjects();

	return UPDATE_CONTINUE;
}

void SceneManager::OpenCloseConfigSceneWindow()
{
	config_scene = !config_scene;
}

void SceneManager::DrawMode() const
{
	GLint polygonMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	if (draw_mode == DM_WIREFRAME && *polygonMode != GL_LINE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (draw_mode == DM_NORMAL && *polygonMode != GL_FILL)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SceneManager::ApplyToMaterial(Texture * new_text, int num_material)
{
	Material* material = GetMaterial(num_material);
	if (material != nullptr)
		material->AddTexture(new_text);
	else
		LOG("Can not apply texture %s to material number %i, material non exsistent", new_text->GetName().c_str(), num_material);
}

void SceneManager::PushBackAssets(const std::vector<Asset*>& assets_to_add)
{
	assets.reserve(assets_to_add.size());
	for (std::vector<Asset*>::const_iterator it = assets_to_add.begin(); it != assets_to_add.end(); ++it)
		assets.push_back((*it));
}

unsigned int SceneManager::GetNumMaterials() const
{
	unsigned int num_materials = 0;

	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetType() == AT_MATERIAL)
			num_materials++;

	return num_materials;
}

Material * SceneManager::GetMaterial(unsigned int pos) const
{
	unsigned int num_materials = 0;

	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if ((*it)->GetType() == AT_MATERIAL)
		{
			if (num_materials == pos)
				return (Material*)*it;
			num_materials++;
		}
	}

	return nullptr;
}

Material * SceneManager::GetMaterial(const std::string & name) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetType() == AT_MATERIAL)
			if (name == (*it)->GetName())
				return (Material*)*it;

	return nullptr;
}

void SceneManager::DeleteMaterial(Material* material_to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (*it == material_to_delete)
		{
			delete *it;
			return;
		}
}

bool SceneManager::HasMaterials() const
{
	return GetNumMaterials() > 0;
}

void SceneManager::CalculateDistanceToObj(const GameObject* go, vec3& center, float& x_dist, float& y_dist, float& z_dist) const
{
	AABB bounding_box(vec(0, 0, 0), vec(0, 0, 0));
	//go->GenerateBoundingBox(bounding_box);
	center.x = bounding_box.CenterPoint().x;
	center.y = bounding_box.CenterPoint().y;
	center.z = bounding_box.CenterPoint().z;
	x_dist = bounding_box.MaxX() - bounding_box.MinX();
	y_dist = bounding_box.MaxY() - bounding_box.MinY();
	z_dist = bounding_box.MaxZ() - bounding_box.MinZ();
}

const GameObject * SceneManager::GetFocused() const
{
	return focused;
}

Mesh * SceneManager::CreateMesh(const char * const name)
{
	Mesh* new_mesh;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name, "Mesh %i", next_mesh);
		new_mesh = new Mesh(new_name);
	}
	else
		new_mesh = new Mesh(name);

	next_mesh++;
	assets.push_back(new_mesh);
	return new_mesh;
}

Mesh * SceneManager::GetMesh(unsigned int pos) const
{
	unsigned int num_meshes = 0;

	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
	{
		if ((*it)->GetType() == AT_MESH)
		{
			if (num_meshes == pos)
				return (Mesh*)*it;
			num_meshes++;
		}
	}

	return nullptr;
}

Mesh * SceneManager::GetMesh(const std::string & name) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetType() == AT_MESH)
			if (name == (*it)->GetName())
				return (Mesh*)*it;

	return nullptr;
}

void SceneManager::DeleteMesh(Mesh * mesh_to_delete)
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if (*it == mesh_to_delete)
		{
			delete *it;
			return;
		}
}

void SceneManager::EmptyScene()
{
	delete root;
	root = new GameObject(nullptr, "Root");
	focused = root;
}

GameObject* SceneManager::CreateGameObject(const char* const name)
{
	GameObject* new_go = focused->CreateChild(name);
	num_game_objects++;
	go_kdtree->AddGameObject(new_go);
	return new_go;
}

GameObject * SceneManager::CreateGameObject(GameObject * parent, const char * const name)
{
	GameObject* new_go = parent->CreateChild(name);
	num_game_objects++;
	go_kdtree->AddGameObject(new_go);
	return new_go;
}

void SceneManager::DrawKDT() const
{
	go_kdtree->Draw();
}

bool SceneManager::AddToKDT(GameObject* new_go)
{
	return go_kdtree->AddGameObject(new_go);
}

void SceneManager::Hirarchy()
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((focused == root) ? ImGuiTreeNodeFlags_Selected : 0);

	if (ImGui::Begin("Hirarchy", &hirarchy_active))
	{
		root->Hirarchy(focused);

		ImGui::End();
	}

	if (focused != root)
		focused->Inspector();
}

void SceneManager::UpdateGameObjects()
{
	root->Update();
}

Material * SceneManager::CreateMaterial(const char * const name)
{
	Material* new_material;

	if (name == nullptr)
	{
		char new_name[255];
		sprintf(new_name,"Material %i", next_material);
		new_material = new Material(new_name, ++material_priority);
	}
	else
		new_material = new Material(name, ++material_priority);

	next_material++;	
	assets.push_back(new_material);
	return new_material;
}
/*
Texture * SceneManager::LoadTextureStraightFromPath(const std::string & path)
{
	if (!Exsists(path))
	{
		//Assume 2D difuse
		Texture* new_texture = CreateTexture(path, TT_DIFFUSE, GL_TEXTURE_2D, 0);

		if (!texture_inporter->LoadTexture(path, *new_texture))
		{
			delete new_texture;
			LOG("Error loading texture '%s'\n", path.c_str());
			return nullptr;
		}
		else
			return new_texture;
	}
	else
		LOG("Texture already exsists");

	return nullptr;
}*/

void SceneManager::AddTexture(Texture * new_texture)
{
	assets.push_back(new_texture);
}

Texture * SceneManager::GetTexture(unsigned int i) const
{
	unsigned int num_texture = 0;
	unsigned int num_asset = 0;

	while (num_texture < i && num_asset < assets.size())
	{
		if (assets[num_asset]->GetType() == AT_TEXTURE)
		{
			if (num_texture == i)
				return (Texture*)assets[num_asset];
			num_texture++;
		}
		num_asset++;
	}

	LOG("Texture %i doesn't exsist, there are only %i textures", i, num_texture);
	return nullptr;
}

const int SceneManager::GetTextureToDraw() const
{
	return texture_to_draw;
}

bool SceneManager::DebugTextures() const
{
	return debug_textures;
}

bool SceneManager::TextureExsists(const std::string & path) const
{
	std::string new_name;
	size_t start = path.find_last_of("//");
	size_t count = path.find_last_of(".") - start;

	// make sure the poisition is valid
	if (start == path.length())
		LOG("Coud not create texture name");
	else
		new_name = path.substr(start + 1, count);

	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if((*it)->GetType() == AT_TEXTURE)
			if ((*it)->GetName() == new_name)
				return true;
	return false;
}

void SceneManager::DrawTexture(unsigned int num_texture) const
{
	Texture* texture = GetTexture(num_texture);
	if (texture < nullptr)
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, texture->GetID());

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
	else
		LOG("Can't draw texture, texture is non exsistant");
}

unsigned int SceneManager::GetNumTextures() const
{
	unsigned int num_textures = 0;

	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it)->GetType() == AT_TEXTURE)
			num_textures++;

	return num_textures;
}

Texture * SceneManager::GetTexture(const std::string & name) const
{
	for (std::vector<Asset*>::const_iterator it = assets.begin(); it != assets.end(); ++it)
		if((*it)->GetType() == AT_TEXTURE)
			if ((*it)->GetName() == name)
				return (Texture*)(*it);

	return nullptr;
}

void SceneManager::DeleteTexture(Texture * texture_to_delete)
{
	for (std::vector<Asset*>::iterator it = assets.begin(); it != assets.end(); ++it)
		if ((*it) == texture_to_delete)
		{
			delete (*it);
			assets.erase(it);
			break;
		}
}

Texture * SceneManager::CreateTexture(const std::string & name, const TEXTURE_TYPE type, const GLenum gl_texure_type, const GLuint & id)
{
	Texture* new_texture = new Texture(name, type, gl_texure_type, id);
	assets.push_back(new_texture);
	return new_texture;
}
