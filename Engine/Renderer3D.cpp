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
#include "Renderer3D.h"

//#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */


void Renderer3D::DrawCubeWithVertexArrays()
{
	glBegin(GL_TRIANGLES);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Front	
	glVertex3f(0.f, 0.f, 0.f);		//B
	glVertex3f(0.f, 0.f, 1.f);		//C
	glVertex3f(0.f, 1.f, 0.f);		//A

	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(0.f, 0.f, 1.f);		//C
	glVertex3f(0.f, 1.f, 1.f);		//D

									//Right
	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(0.f, 1.f, 1.f);		//D
	glVertex3f(0.f, 0.f, 1.f);		//C

	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(0.f, 0.f, 1.f);		//C
	glVertex3f(1.f, 0.f, 1.f);		//F

									//Left
	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(0.f, 0.f, 0.f);		//B
	glVertex3f(0.f, 1.f, 0.f);		//A

	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(1.f, 1.f, 0.f);		//G

									//Bottom
	glVertex3f(0.f, 0.f, 0.f);		//B
	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(0.f, 0.f, 1.f);		//C

	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 0.f, 1.f);		//F
	glVertex3f(0.f, 0.f, 1.f);		//C

									//Top
	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(1.f, 1.f, 0.f);		//G

	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(0.f, 1.f, 1.f);		//D
	glVertex3f(1.f, 1.f, 1.f);		//E

									//Behind
	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 1.f, 0.f);		//G
	glVertex3f(1.f, 1.f, 1.f);		//E

	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(1.f, 0.f, 1.f);		//F

	glEnd();
}

void Renderer3D::DrawWorldAxis()
{
	glLineWidth(2.0f);

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
}

Renderer3D::Renderer3D(Application* app, bool start_enabled) : Module(app, "Renderer3D", start_enabled)
{}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Init()
{
	App->LOG("Creating 3D Renderer context");
	bool ret = true;
	
	JSON_Value* config = json_parse_file("config.json");
	JSON_Object* obj = json_value_get_object(config);
	JSON_Object* rend = json_object_dotget_object(obj, "3DRenderer");
	bool VSYNC = json_object_get_boolean(rend, "VSYNC");

	if (!ImGui_ImplSdlGL2_Init(App->window->window))
	{
		App->LOG("ImGui could not be Initialized!");
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			App->LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		/*//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, App->window->GetWidth(), 0, App->window->GetHeight(), 0.0f, 1.0f);
		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}*/

		/*//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}*/
		
		/*glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
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
		glEnable(GL_TEXTURE_2D);*/
	}

	// Projection matrix for
	/*int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	OnResize(w, h);*/

	return ret;
}

// PreUpdate: clear buffer
UPDATE_STATUS Renderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PreUpdate", Profiler::Color::Aqua)

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(App->camera->GetViewMatrix());

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UPDATE_STATUS Renderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PostUpdate", Profiler::Color::AntiqueWhite)

	//ImVec4 clear_color = ImColor(25, 25, 25);
	//ImGuiIO io = ImGui::GetIO();
	//glViewport(0, 0, (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x), (int)(io.DisplaySize.y* io.DisplayFramebufferScale.y));
	
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	//first geometry, then debug and then UI

	//Vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);

	App->open_gl_test->DrawTriangle();
	App->open_gl_test->DrawQuad();
	App->open_gl_test->Draw2DPoint();
	App->open_gl_test->DrawLine();
	/*
	if (debug_draw == true)
	{
		BeginDebugDraw();
		App->DebugDraw();
		EndDebugDraw();
	}*/

	//DrawCubeWithVertexArrays();
	//DrawWorldAxis();

	glDisableClientState(GL_VERTEX_ARRAY);
	//------

	ImGui::Render();
	
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	App->LOG("Destroying 3D Renderer");
	return true;
}


void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	//glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());
}
