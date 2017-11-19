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

	root = new GameObject("Root", false);
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

		if (ImGui::Button("Add Empty GO"))
			AddEmptyGO();

		ImGui::Checkbox("Draw Game Objects' KDT", &draw_kdt);
	}
	App->EndDockWindow();

	ImGui::ShowTestWindow();

	Hirarchy();

	return ret;
}

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

GameObject * SceneManager::GetFocused() const
{
	return focused;
}

void SceneManager::AddPrefabToFocused(GameObject * root)
{
	focused->AddChild(root);
}

void SceneManager::AddPrefabAsNewGameObjects(GameObject * root)
{
	focused->AddChild(new GameObject(*root));
}

void SceneManager::UpdateAABBs(const GameObject* go) const
{
	root->UpdateAABBs(go);
}

void SceneManager::AddEmptyGO()
{
	char name[256];
	sprintf(name, "Game_Object_%i", num_game_objects++);

	GameObject* new_go = new GameObject(name);
	if (AddToKDT(new_go) == true)
		focused->AddChild(new_go);
	else
		delete new_go;
}

void SceneManager::UpdateKDT(const GameObject * go)
{
	go_kdtree->UpdateGO(go);
}

void SceneManager::EmptyScene()
{
	delete root;
	root = new GameObject("Root");
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
	if(draw_kdt)
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

