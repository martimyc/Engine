#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Application.h"
#include "Renderer3D.h"
#include "Mesh.h"
#include "Material.h"
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

	for (std::vector<Material*>::iterator it = materials.begin(); it != materials.end(); ++it)
		delete (*it);
	materials.clear();

	return true;
}

UPDATE_STATUS SceneManager::Configuration(float dt)
{
	BROFILER_CATEGORY("Scene Manager Configuration", Profiler::Color::BlanchedAlmond)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Draw Modes"))
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

	if (ImGui::CollapsingHeader("Materials"))
	{
		std::vector<int> materials_to_delete;

		ImGui::Text("Materials:");
		for (int i = 0; i < materials.size(); i++)
		{
			char mesh_name[255];
			sprintf(mesh_name, "Material: %i", i);

			if (ImGui::TreeNode(mesh_name))
			{
				materials[i]->Configuration();

				ImGui::InputInt("Mesh:", &mesh_change_num);
				ImGui::SameLine();
				if (ImGui::Button("Make mesh material"))
					App->scene_manager->game_object->ChangeMaterial(materials[i], mesh_change_num);

				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					LOG("Deleting textures");
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

	game_object->Configuration();
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

void SceneManager::CalculateDistanceToObj(const GameObject* go, vec3& center, float& x_dist, float& y_dist, float& z_dist) const
{
	AABB bounding_box(vec(0, 0, 0), vec(0, 0, 0));
	go->GenerateBoundingBox(bounding_box);
	center.x = bounding_box.CenterPoint().x;
	center.y = bounding_box.CenterPoint().y;
	center.z = bounding_box.CenterPoint().z;
	x_dist = bounding_box.MaxX() - bounding_box.MinX();
	y_dist = bounding_box.MaxY() - bounding_box.MinY();
	z_dist = bounding_box.MaxZ() - bounding_box.MinZ();
}

void SceneManager::EmptyScene()
{
	delete game_object;
	game_object = new GameObject();
}
