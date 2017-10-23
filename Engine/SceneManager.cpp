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
#include "TextureInporter.h"
#include "SceneInporter.h"
#include "SceneManager.h"

SceneManager::SceneManager(const char * name, bool start_enabled) : Module(name, start_enabled), draw_mode(DM_NORMAL), wireframe(false), normals(false), polygons(true)
{}

SceneManager::~SceneManager()
{}

bool SceneManager::Init()
{
	texture_inporter = new TextureInporter();
	scene_inporter = new SceneInporter();

	//checkers texture
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

	Texture* checkers = new Texture(std::string("Checkers"), TT_DIFFUSE, GL_TEXTURE_2D, checkers_text_id);
	checkers->width = CHECKERS_WIDTH;
	checkers->height = CHECKERS_HEIGHT;
	AddTexture(checkers);

	return true;
}

bool SceneManager::CleanUp()
{
	for (std::vector<Material*>::iterator it = materials.begin(); it != materials.end(); ++it)
		delete (*it);
	materials.clear();

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

	if(ImGui::Begin("Assets", &assets_enable))
	{
		MaterialsConfiguration(dt);
		TexturesConfiguration(dt);
		ImGui::End();
	}

	game_objects.Hirarchy();

	return ret;
}

UPDATE_STATUS SceneManager::MaterialsConfiguration(float dt)
{
	if (ImGui::CollapsingHeader("Materials"))
	{
		if (ImGui::Button("Create material"))
			CreateMaterial();

		std::vector<int> materials_to_delete;

		ImGui::Text("Materials:");
		for (int i = 0; i < materials.size(); i++)
		{
			char mesh_name[255];
			sprintf(mesh_name, "Material: %i", i);

			if (ImGui::TreeNode(mesh_name))
			{
				materials[i]->LoneConfig();

				if (ImGui::InputInt("Mesh:", &current_mesh))
				{
					if (current_mesh < 0)
						current_mesh = 0;
					else if (current_mesh > game_objects.GetFocused()->GetNumMeshes())
					{
						LOG("Mesh %i does not exsist in this game gbject");
						current_mesh = game_objects.GetFocused()->GetNumMeshes();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Apply"))
					if (game_objects.GetFocused() != nullptr)
						game_objects.GetFocused()->ChangeMaterial(materials[i], current_mesh);
					else
						LOG("Can not add material, no focused game object");

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

	return UPDATE_CONTINUE;
}

UPDATE_STATUS SceneManager::TexturesConfiguration(float dt)
{
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::Button("Empty"))
			EmptyTextures();

		if (ImGui::Button("Debug Textures"))
			debug_textures = !debug_textures;

		if (debug_textures)
		{
			if (ImGui::InputInt("Texture to draw", &texture_to_draw))
			{
				if (texture_to_draw >= textures.size())
				{
					LOG("Texture %i does not exist, binding Checkers", texture_to_draw);
					texture_to_draw = 0;
				}
				else if (texture_to_draw < 0)
					texture_to_draw = 0;
			}
		}

		std::vector<int> textures_to_delete;
		bool node_open = false;

		for (int i = 0; i < textures.size(); i++)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selected_texture == i) ? ImGuiTreeNodeFlags_Selected : 0);

			node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, textures[i]->name.c_str());

			if (ImGui::IsItemClicked())
				selected_texture = i;

			if(node_open)
			{
				ImGui::Text("Path: %s", textures[i]->path.c_str());

				ImVec2 size(50,50);
				ImVec2 uv0(0, 1);
				ImVec2 uv1(1, 0);

				ImGui::Image((void*) textures[i]->id, size, uv0, uv1);
				ImGui::SameLine();
				ImGui::Text("Width: %i\nHeight: %i", textures[i]->width, textures[i]->height);

				if (ImGui::Button("Delete"))
				{
					LOG("Deleting textures");
					textures_to_delete.push_back(i);
				}
				
				ImGui::TreePop();
			}
		}

		char add[255];
		sprintf(add, "Add %s to material", textures[selected_texture]->name.c_str());

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
}

UPDATE_STATUS SceneManager::Update(float dt)
{
	game_objects.Draw();

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

void SceneManager::ApplyToMaterial(Texture * new_text, int material)
{
	materials[material]->AddTexture(new_text);
}

void SceneManager::ReserveMaterialSpace(const GLuint & num_materials)
{
	materials.reserve(num_materials * sizeof(Material*));
}

bool SceneManager::DrawNormals() const
{
	return normals;
}

unsigned int SceneManager::NumMaterials() const
{
	return materials.size();
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

const GameObject * SceneManager::GetFocused() const
{
	return game_objects.GetFocused();
}

void SceneManager::EmptyScene()
{
	game_objects.Empty();
}


GameObject* SceneManager::CreateGameobject(const char* const name)
{
	return game_objects.CreateGameobject(name);
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
	materials.push_back(new_material);
	return new_material;
}

void SceneManager::DeleteMaterial(const Material * to_delete)
{
	for (std::vector<Material*>::iterator it = materials.begin(); it != materials.end(); ++it)
		if ((*it) == to_delete)
		{
			delete (*it);
			materials.erase(it);
			break;
		}
}

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
}

void SceneManager::AddTexture(Texture * new_texture)
{
	textures.push_back(new_texture);
}

void SceneManager::EmptyTextures()
{
	for (std::vector<Texture*>::iterator it = textures.begin() + 1; it != textures.end(); ++it)
		delete (*it);
	textures.clear();
}

Texture * SceneManager::GetCheckers() const
{
	return textures[0];
}

Texture * SceneManager::GetTexture(unsigned int i) const
{
	return textures[i];
}

const int SceneManager::GetTextureToDraw() const
{
	return texture_to_draw;
}

bool SceneManager::DebugTextures() const
{
	return debug_textures;
}

bool SceneManager::Exsists(const std::string & path) const
{
	std::string new_name;
	size_t start = path.find_last_of("//");
	size_t end = path.find_last_of(".");
	// make sure the poisition is valid
	if (start == path.length() || end == path.length())
		LOG("Coud not create texture name");
	else
		new_name = path.substr(start + 1, end);

	for (std::vector<Texture*>::const_iterator it = textures.begin(); it != textures.end(); ++it)
		if ((*it)->path == new_name)
			return true;
	return false;
}

void SceneManager::DrawTexture(unsigned int num_texture) const
{
	if (num_texture < textures.size())
	{
		// Select the texture to use
		glBindTexture(GL_TEXTURE_2D, textures[num_texture]->id);

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
		LOG("Can't draw texture, last existing texture is %i", textures.size() - 1);
}

Texture * SceneManager::CreateTexture(const std::string & path, const TEXTURE_TYPES type, const GLenum dimensions, const GLuint & id)
{
	Texture* new_texture = new Texture(path, type, dimensions, id);
	textures.push_back(new_texture);
	return new_texture;
}

bool SceneManager::LoadScene(const std::string & path) const
{
	return scene_inporter->LoadScene(path);
}

