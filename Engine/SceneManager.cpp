#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Application.h"
#include "Renderer3D.h"
#include "Mesh.h"
#include "BasicGeometry.h"
#include "GameObject.h"
#include "Texture.h"
#include "TextureManager.h"
#include "SceneManager.h"

SceneManager::SceneManager(const char * name, bool start_enabled) : Module(name, start_enabled), draw_mode(DM_NORMAL), wireframe(false), normals(false), polygons(true)
{}

SceneManager::~SceneManager()
{}

bool SceneManager::Init()
{
	game_object = new GameObject();

	return true;
}

bool SceneManager::CleanUp()
{
	delete game_object;
	return true;
}

UPDATE_STATUS SceneManager::Configuration(float dt)
{
	BROFILER_CATEGORY("Scene Manager Configuration", Profiler::Color::BlanchedAlmond)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Draw Modes"))
	{
		if(ImGui::Checkbox("Wireframe", &wireframe))
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
			if(normals)
				draw_mode = DM_NORMALS;
			else
			{
				if(wireframe)
					draw_mode = DM_WIREFRAME;
				else if(polygons)
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

		if (ImGui::Button("Checkers"))
		{
			game_object->ApplyTexture(App->texture_manager->GetCheckers());
		}
		if (ImGui::Button("Texture 2"))
		{
			game_object->ApplyTexture(App->texture_manager->GetSecondTexture());
		}
	}
	return ret;
}

UPDATE_STATUS SceneManager::Update(float dt)
{
	if (game_object != nullptr)
	{
		App->renderer_3d->DrawGO(game_object);
	}

	return UPDATE_CONTINUE;
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

void SceneManager::AddMaterial(Material* new_material)
{
	materials.push_back(new_material);
}

void SceneManager::ReserveMaterialSpace(const GLuint & num_materials)
{
	materials.reserve(num_materials * sizeof(Material*));
}

Material * SceneManager::GetMaterial(unsigned int pos) const
{
	return materials[pos];
}

bool SceneManager::HasMaterials() const
{
	return materials.size() > 0;
}

float SceneManager::CalculateDistanceToObj(const GameObject* go) const
{
	AABB bounding_box;
	go->GenerateBoundingBox(bounding_box);
	if (bounding_box.MinY() <= -6e18 || bounding_box.MinY() >= 6e18)	//error from if height == 0
		return bounding_box.MaxY();
	return bounding_box.MaxY() - bounding_box.MinY();
}
