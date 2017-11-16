#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "MathGeoLib\src\Geometry\AABB.h"

//Components & Assets
#include "AppliedMaterial.h"
#include "MeshFilter.h"
#include "Transform.h"
#include "Camera.h"

#include "KDTreeGO.h"

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
	go_kdtree = new KDTreeGO();

	root = new GameObject(nullptr, "Root");
	focused = root;

	go_kdtree->AddGameObject(root);

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

	ImGui::ShowTestWindow();

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

void SceneManager::SetFocused(GameObject* game_obj)
{
	focused = game_obj;
}

const GameObject * SceneManager::GetFocused() const
{
	return focused;
}

void SceneManager::AddPrefabToFocused(GameObject * root)
{
	focused->AddChild(root);
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
	if (go_kdtree->AddGameObject(new_go) == false)
	{
		LOG("GameObject: '%s' could not be added to KDT", new_go->GetName().c_str());
		focused->Delete(new_go);
		return nullptr;
	}
	return new_go;
}

GameObject * SceneManager::CreateGameObject(GameObject * parent, const char * const name)
{
	GameObject* new_go = parent->CreateChild(name);
	num_game_objects++;
	if (go_kdtree->AddGameObject(new_go) == false)
	{
		LOG("GameObject: '%s' could not be added to KDT", new_go->GetName().c_str());
		parent->Delete(new_go);
		return nullptr;
	}
	return new_go;
}

void SceneManager::RemoveWithChilds(GameObject * to_remove)
{
	root->Delete(to_remove);
}

void SceneManager::DrawKDT() const
{
	go_kdtree->Draw();
}

bool SceneManager::AddToKDT(GameObject* new_go)
{
	return go_kdtree->AddGameObject(new_go);
}

void SceneManager::CreateCamera()
{
	root->CreateCamera();
}

GameObject * SceneManager::GetRoot() const
{
	return root;
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
	root->SentToDraw(focused_camera);
}

void SceneManager::DrawCamera()
{
	if (focused_camera != nullptr)
		focused_camera->DrawFrustum();
}

void SceneManager::SetCameraFocused(Camera * camera)
{
	focused_camera = camera;
}

