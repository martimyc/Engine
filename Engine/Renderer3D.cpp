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
#include "OpenGLTest.h"
#include "GameObject.h"
#include "SceneManager.h"
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

	if (ImGui::CollapsingHeader("3D Renderer"))
	{
		ImGui::Checkbox("Draw World Axis", &world_axis);
		ImGui::Checkbox("Draw Grid", &show_grid);
		ImGui::ColorEdit3("Grid Color", grid_color); 
		ImGui::SliderInt("Grid divisions", &grid_divisions, 5, 40);
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

	if (draw_vec.size() != 0)
	{
		LOG("Draw vector is not empty at frame start");
		draw_vec.clear();
	}

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
	DRAW_MODE draw_mode = App->scene_manager->GetDrawMode();

	for (std::vector<const GameObject*>::const_iterator it = draw_vec.begin(); it != draw_vec.end(); ++it)
		(*it)->Draw(draw_mode);

	/*if (debug_draw == true)
	{
		BeginDebugDraw();
		App->DebugDraw();
		EndDebugDraw();
	}*/

	App->open_gl_test->DrawDebugPoint();

	if (show_grid)
		DrawGrid();
	if (world_axis)
		DrawWorldAxis();

	//------

	ImGui::Render();
	
	SDL_GL_SwapWindow(App->window->window);

	//Empty draw_vec after drawing all Game Objects
	draw_vec.clear();

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

void Renderer3D::DrawGO(const GameObject* game_object)
{
	draw_vec.push_back(game_object);
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
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	int j = grid_divisions;

	glColor4f(grid_color[0], grid_color[1], grid_color[2], 1.0f);

	for (int i = j*(-1); i <= j; i++)
	{
		if (i == 0)
			glColor4f(255, 255, 255, 1.0f);

		glVertex3f(i, 0.0f, j);
		glVertex3f(i, 0.0f, -j);
		glVertex3f(j, 0.0f, i);
		glVertex3f(-j, 0.0f, i);

		if (i == 0)
			glColor4f(grid_color[0], grid_color[1], grid_color[2], 1.0f);
	}

	glEnd();
}