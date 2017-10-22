#include "Globals.h"
#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Parson\parson.h"
#include "Application.h"
#include "Window.h"
#include "Camera3D.h"
#include "Console.h"
#include "Mesh.h"
#include "Material.h"
#include "SceneManager.h"
#include "TextureInporter.h"
#include "Texture.h"
#include "Renderer3D.h"

Renderer3D::Renderer3D(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* rend = json_object_dotget_object(obj, "3DRenderer");
	bool VSYNC = json_object_get_boolean(rend, "VSYNC");

	if (!ImGui_ImplSdlGL2_Init(App->window->window))
	{
		LOG("ImGui could not be Initialized!");
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho(0, App->window->GetWidth(), 0, App->window->GetHeight(), 0.0f, 1.0f);
		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}
	
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	}

	// Projection matrix for
	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	OnResize(w, h);

	return ret;
}

UPDATE_STATUS Renderer3D::Configuration(float dt)
{
	BROFILER_CATEGORY("Renderer3D Configuration", Profiler::Color::Beige)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Renderer 3D"))
	{
		ImGui::Checkbox("Draw World Axis", &world_axis);
		ImGui::Checkbox("Draw Grid", &show_grid);
		ImGui::ColorEdit3("Grid Color", grid_color); 
		ImGui::SliderInt("Grid divisions", &grid_divisions, 5, 40);
		ImGui::SliderFloat("Grid line width", &grid_line_width, 1.0, 5.0);
		ImGui::SliderFloat("Grid size", &grid_size, 0.1, 2.5);
	}
	return ret;
}

// PreUpdate: clear buffer
UPDATE_STATUS Renderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PreUpdate", Profiler::Color::Aqua)

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	if (draw_queue.size() != 0)
		LOG("Draw vector is not empty at frame start");

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UPDATE_STATUS Renderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PostUpdate", Profiler::Color::AntiqueWhite)

	ImVec4 clear_color = ImColor(25, 25, 25);
	ImGuiIO io = ImGui::GetIO();
	glViewport(0, 0, (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x), (int)(io.DisplaySize.y* io.DisplayFramebufferScale.y));
	
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	//first geometry, then debug and then UI

	Anisotrophy();

	//Set drawing mode if changed
	App->scene_manager->DrawMode();

	//meshes
	Material* material_in_use;
	while (draw_queue.size() > 0)
	{
		material_in_use = draw_queue.top()->GetMaterial();
		if(material_in_use != nullptr)
			material_in_use->EnableDraw();
		while (draw_queue.size() > 0 && draw_queue.top()->GetMaterial() == material_in_use)
		{
			draw_queue.top()->Draw();
			draw_queue.pop();
		}
		if (material_in_use != nullptr)
			material_in_use->DisableDraw();
	}

	//for (std::vector<const GameObject*>::const_iterator it = draw_vec.begin(); it != draw_vec.end(); ++it)
		//(*it)->Draw();

	/*if (debug_draw == true)
	{
		BeginDebugDraw();
		App->DebugDraw();
		EndDebugDraw();
	}*/

	//App->open_gl_test->DrawDebugPoint();
	//App->open_gl_test->DrawPlaneExtraLarge();

	//Debug Textures
	if (App->scene_manager->DebugTextures())
		App->scene_manager->DrawTexture(App->scene_manager->GetTextureToDraw());

	if (show_grid)
		DrawGrid();
	if (world_axis)
		DrawWorldAxis();
	//------

	ImGui::Render();
	
	SDL_GL_SwapWindow(App->window->window);

	//Empty draw_vec after drawing all Game Objects
	if (draw_queue.size() != 0)
		LOG("Draw queue not empty at end of frame");

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	return true;
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());
}

void Renderer3D::DrawMesh(Mesh* mesh)
{
	draw_queue.push(mesh);
}

void Renderer3D::Anisotrophy()
{
	glEnableClientState(GL_TEXTURE_2D);

	if(App->scene_manager->GetTexture(0) != nullptr)
		glBindTexture(GL_TEXTURE_2D, App->scene_manager->GetTexture(0)->id);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glTexCoord2f(0.0f, 50.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glTexCoord2f(50.0f, 50.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glTexCoord2f(50.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_TEXTURE_2D);
}

void Renderer3D::DrawWorldAxis()
{
	glLineWidth(4.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	glEnd();

	glLineWidth(1.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer3D::DrawGrid()
{
	glLineWidth(grid_line_width);

	glBegin(GL_LINES);

	int j = grid_divisions;

	glColor4f(grid_color[0], grid_color[1], grid_color[2], 1.0f);

	for (int i = j*(-1); i <= j; i++)
	{
		if (i == 0)
			glColor4f(255, 255, 255, 1.0f);

		glVertex3f(i * grid_size, 0.0f, j * grid_size);
		glVertex3f(i * grid_size, 0.0f, -j * grid_size);
		glVertex3f(j * grid_size, 0.0f, i * grid_size);
		glVertex3f(-j * grid_size, 0.0f, i * grid_size);

		if (i == 0)
			glColor4f(grid_color[0], grid_color[1], grid_color[2], 1.0f);
	}

	glEnd();

	glLineWidth(1.0f);
	glColor4f(255, 255, 255, 1.0f);
}

bool CompareMeshPointers::operator()(const Mesh * m1, const Mesh * m2)
{
	unsigned int priority_one = ((m1->GetMaterial() == nullptr) ? 0 : m1->GetMaterial()->GetPriority());
	unsigned int priority_two = ((m2->GetMaterial() == nullptr) ? 0 : m2->GetMaterial()->GetPriority());
	return priority_one > priority_two;
}
