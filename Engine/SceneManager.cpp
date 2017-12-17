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
#include "Input.h"
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

	config_scene = false;

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

	Hirarchy();

	if (focused->IsSetToDelete())
	{
		GameObject* parent_focused = focused->GetParent();
		focused->EraseFromKDTree(go_kdtree);
		parent_focused->Delete(focused);
		focused = parent_focused;
	}

	return ret;
}

UPDATE_STATUS SceneManager::Update(float dt)
{
	UpdateGameObjects();

	float4 vec(0.0f, 0.0f, 1.0f, 1.0f);
	float4x4 mat(focused->GetWorldTransform());
	mat.Transpose();
	vec = mat * vec;

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (focused != root)
			focused->SetToDelete();
	}

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

const DRAW_MODE SceneManager::GetDrawMode() const
{
	return draw_mode;
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
	GameObject* new_game_object = new GameObject(*root);
	if (AddToKDT(new_game_object) != false)
		focused->AddChild(new_game_object);
	else
		LOG("Prefab could not be added to the KD-Tree");
}

void SceneManager::DrawBoundingBoxes() const
{
	root->DrawBoundingBoxes();
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

void SceneManager::DeleteFocused()
{
	GameObject* to_delete = focused;
	focused = focused->GetParent();
	focused->Delete(to_delete);
}

void SceneManager::SaveGameObjects(char ** iterator) const
{
	root->SaveGameObjects(iterator);
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

GameObject * SceneManager::GetGO(const std::string & name) const
{
	return root->GetChild(name);
}

void SceneManager::Hirarchy()
{
	if (App->BeginDockWindow("Hirarchy", &hirarchy_active))
	{
		root->Hirarchy(focused);
	}
	App->EndDockWindow();

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
