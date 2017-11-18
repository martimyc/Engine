#include "Globals.h"
#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl.h"
#include "Brofiler\Brofiler.h"
#include "Parson\parson.h"

//components & assets
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "AppliedMaterial.h"
#include "MeshFilter.h"
#include "Texture.h"

//Modules
#include "Window.h"
#include "Camera3D.h"
#include "Console.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Renderer3D.h"

Renderer3D::Renderer3D(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

// Destructor
Renderer3D::~Renderer3D()
{
}

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

	render_to_texture = new FrameBuffer();
	render_to_texture->CreateFrameBuffer(App->window->GetWidth(), App->window->GetHeight());

	play_buttons_size_x = 205;
	play_buttons_size_y = 75;

	return ret;
}

UPDATE_STATUS Renderer3D::Configuration(float dt)
{
	BROFILER_CATEGORY("Renderer3D Configuration", Profiler::Color::Beige)

	UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (App->BeginDockWindow("Renderer 3D", &config_renderer))
	{
		ImGui::Checkbox("Draw Mouse Picking", &show_ray_picking);
		ImGui::Checkbox("Draw World Axis", &world_axis);
		ImGui::Checkbox("Draw Grid", &show_grid);
		ImGui::ColorEdit3("Grid Color", grid_color); 
		ImGui::SliderInt("Grid divisions", &grid_divisions, 5, 40);
		ImGui::SliderFloat("Grid line width", &grid_line_width, 1.0, 5.0);
		ImGui::SliderFloat("Grid size", &grid_size, 0.1, 2.5);
	}
	App->EndDockWindow();

	return ret;
}

// PreUpdate: clear buffer
UPDATE_STATUS Renderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PreUpdate", Profiler::Color::Aqua);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	math::float4x4 view_matrix = App->camera->GetViewMatrix();
	const GLfloat* opengl_view_matrix = (GLfloat*)view_matrix.ptr();
	glLoadMatrixf(opengl_view_matrix);

	if (draw_queue.size() != 0)
		LOG("Draw vector is not empty at frame start");

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UPDATE_STATUS Renderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer PostUpdate", Profiler::Color::AntiqueWhite)

	//Get matrix for all camera uses
	math::float4x4 view_matrix = App->camera->GetViewMatrix();
	const GLfloat* opengl_view_matrix = (GLfloat*)view_matrix.ptr();

	render_to_texture->BindFrameBuffer();

	ImVec4 clear_color = ImColor(25, 25, 25);
	ImGuiIO io = ImGui::GetIO();
	glViewport(0, 0, (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x), (int)(io.DisplaySize.y* io.DisplayFramebufferScale.y));

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	//first geometry, then debug and then UI
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(opengl_view_matrix);

	//Set drawing mode if changed
	App->scene_manager->DrawMode();

	//Camera
	App->scene_manager->DrawCamera();

	if (show_ray_picking)
		App->camera->DrawPickingRay();
	if (show_grid)
		DrawGrid();
	if (world_axis)
		DrawWorldAxis(); 
	glPopMatrix();

	//meshes
	const Material* material_in_use = nullptr;
	while (draw_queue.size() > 0)
	{
		material_in_use = draw_queue.top()->GetMaterial();
		if (material_in_use != nullptr)
			material_in_use->EnableDraw();

		while (draw_queue.top()->GetMaterial() == material_in_use)
		{
			glPushMatrix();
			glLoadMatrixf(opengl_view_matrix);

			if (App->camera->DoFrustumCulling(draw_queue.top()))
			{
				draw_queue.top()->DrawBoundingBoxes();	//First bounding boxes, which don't need the transform to be applied
				glMultMatrixf(draw_queue.top()->GetWorldGLTransform());

				draw_queue.top()->GetMesh()->Draw(draw_queue.top()->GetAppliedMaterial());
				draw_queue.top()->GetMesh()->DrawKDT();
			}
			draw_queue.pop();

			glPopMatrix();

			if (draw_queue.size() == 0)
				break;
		}

		if (material_in_use != nullptr)
			material_in_use->DisableDraw();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(opengl_view_matrix);

	//DebugTextures
	//App->resource_manager->DebugTextures();

	App->scene_manager->DrawKDT();

	glPopMatrix();

	//------

	render_to_texture->UnBindFrameBuffer();


	ImGuiWindowFlags flags_scene = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	App->BeginDockWindow("Scene", nullptr, flags_scene);
	
	mouse_on_scene_window = ImGui::IsWindowHovered();
	render_to_texture->SetPosX(ImGui::GetWindowPos().x);
	render_to_texture->SetPosY(ImGui::GetWindowPos().y);
	render_to_texture->SetWidth(ImGui::GetWindowWidth());
	render_to_texture->SetHeight(ImGui::GetWindowHeight());

	//Create Play/Pause/Play1Frame buttons
	ImGui::SetNextWindowPos(ImVec2((render_to_texture->GetPosX() + render_to_texture->GetWidth() / 2) - (play_buttons_size_x / 2), render_to_texture->GetPosY() + (play_buttons_size_y / 2)), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(play_buttons_size_x, play_buttons_size_y), ImGuiCond_Always);
	ImGuiWindowFlags flags_buttons = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("", nullptr, flags_buttons);
	App->resource_manager->CreateButtons();
	ImGui::End();

	ImGui::Image((void*)render_to_texture->GetTextureID(), ImVec2(render_to_texture->GetWidth(), render_to_texture->GetHeight()), ImVec2(0, 1), ImVec2(1, 0));

	App->EndDockWindow();

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

void Renderer3D::OpenCloseConfigRendererWindow()
{
	config_renderer = !config_renderer;
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	App->SetDockContextSize(width, height);

	if (render_to_texture)
	{
		render_to_texture->SetWidth(width);
		render_to_texture->SetHeight(height);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetProjMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewProjMatrix());
}

void Renderer3D::DrawGameObject(const GameObject* game_object)
{
	draw_queue.push(game_object);
}

int Renderer3D::GetScenePosX() const
{
	return render_to_texture->GetPosX();
}

int Renderer3D::GetScenePosY() const
{
	return render_to_texture->GetPosY();
}

int Renderer3D::GetSceneWidth() const
{
	return render_to_texture->GetWidth();
}

int Renderer3D::GetSceneHeight() const
{
	return render_to_texture->GetHeight();
}

bool Renderer3D::IsSceneWindowHovered() const
{
	return mouse_on_scene_window;
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
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

bool CompareGOPointers::operator()(const GameObject * go1, const GameObject * go2)
{
	unsigned int priority_one = ((go1->GetAppliedMaterial() == nullptr) ? 0 : go1->GetAppliedMaterial()->GetMaterial()->GetPriority());
	unsigned int priority_two = ((go2->GetAppliedMaterial() == nullptr) ? 0 : go2->GetAppliedMaterial()->GetMaterial()->GetPriority());
	return priority_one > priority_two;
}

FrameBuffer::FrameBuffer():frame_buffer_id (0), rendered_texture_id (0), depth_render_id(0), width(0), height(0)
{}

FrameBuffer::~FrameBuffer()
{}

void FrameBuffer::CreateFrameBuffer(const int _width, const int _height)
{
	width = _width;
	height = _height;

	// create a texture object
	glGenTextures(1, &rendered_texture_id);
	glBindTexture(GL_TEXTURE_2D, rendered_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a renderbuffer object to store depth info
	glGenRenderbuffers(1, &depth_render_id);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_render_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// create a framebuffer object
	glGenFramebuffers(1, &frame_buffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

	// attach the texture to FBO color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER 
		GL_COLOR_ATTACHMENT0,					// 2. attachment point
		GL_TEXTURE_2D,							// 3. tex target: GL_TEXTURE_2D
		rendered_texture_id,					// 4. tex ID
		0);										// 5. mipmap level: 0(base)

	// attach the renderbuffer to depth attachment point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,		// 1. fbo target: GL_FRAMEBUFFER
		GL_DEPTH_ATTACHMENT,						// 2. attachment point
		GL_RENDERBUFFER,							// 3. rbo target: GL_RENDERBUFFER
		depth_render_id);							// 4. rbo ID


	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindFrameBuffer()
{
	glViewport(0, 0, width, height);
	// set rendering destination to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::UnBindFrameBuffer()
{
	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// trigger mipmaps generation explicitly
	// NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
	// triggers mipmap generation automatically. However, the texture attached
	// onto a FBO should generate mipmaps manually via glGenerateMipmap().
	glBindTexture(GL_TEXTURE_2D, rendered_texture_id);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

GLuint FrameBuffer::GetTextureID()
{
	return rendered_texture_id;
}

uint FrameBuffer::GetWidth() const
{
	return width;
}

uint FrameBuffer::GetHeight() const
{
	return height;
}

uint FrameBuffer::GetPosX() const
{
	return pos_x;
}

uint FrameBuffer::GetPosY() const
{
	return pos_y;
}

void FrameBuffer::SetWidth(const uint width_)
{
	width = width_;
}

void FrameBuffer::SetHeight(const uint height_)
{
	height = height_;
}

void FrameBuffer::SetPosX(const uint x)
{
	pos_x = x;
}

void FrameBuffer::SetPosY(const uint y)
{
	pos_y = y;
}