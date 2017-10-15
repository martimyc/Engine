#include "glew\include\GL\glew.h"
#include "Brofiler\Brofiler.h"
#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\AABB.h"
#include "Globals.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Console.h"
#include "GameObject.h"
#include "Component.h"
#include "Mesh.h"
#include "SceneManager.h"
#include "Camera3D.h"

Camera3D::Camera3D(const char* name, bool start_enabled) : Module( name, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
}

Camera3D::~Camera3D()
{}

bool Camera3D::Init()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, App->window->GetWidth(), 0, App->window->GetHeight(), 0.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/

	return true;
}

// -----------------------------------------------------------------
bool Camera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

UPDATE_STATUS Camera3D::Configuration(float dt)
{
	BROFILER_CATEGORY("Camera3D Configuration", Profiler::Color::Khaki)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Camera 3D"))
	{
		ImGui::SliderFloat("WASD speed", &camera_speed, 2.0f, 15.0f);
		ImGui::SliderFloat("Zoom speed", &camera_zoom_speed, 2.0f, 15.0f);
		ImGui::SliderFloat("Rotation sensivility", &sensitivity, 0.1f, 0.5f);
	}
	return ret;
}

// -----------------------------------------------------------------
bool Camera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
UPDATE_STATUS Camera3D::Update(float dt)
{
	BROFILER_CATEGORY("Camera Update", Profiler::Color::AliceBlue)

		// Implement a debug camera with keys and mouse
		// Now we can make this movememnt frame rate independant!

		vec3 new_pos(0, 0, 0);
	float speed = camera_speed * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = camera_speed * 2 * dt;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) new_pos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) new_pos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) new_pos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) new_pos += X * speed;
	}


	// Mouse whell motion:	-1 equals down, 1 equals up
	if (App->input->GetMouseZ() == -1)
		new_pos += normalize(position) * camera_zoom_speed;

	else if (App->input->GetMouseZ() == 1)
		new_pos -= normalize(position) * camera_zoom_speed;
	

	// Center Obj
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		GameObject* focused_game_object = App->scene_manager->game_object;
		if (focused_game_object != nullptr)
		{
			GLfloat x, y, z;
			focused_game_object->GetWorldPosition(x, y, z);
			float distance_to_focused_obj = App->scene_manager->CalculateDistanceToObj(focused_game_object);			
			position = vec3(x, y, z) + Z * distance_to_focused_obj * 5;
			reference = position;
		}
	}

	position += new_pos;
	reference += new_pos;

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		position -= reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		position = reference + Z * length(position);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void Camera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->position = Position;
	this->reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->reference = this->position;
		this->position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::LookAt( const vec3 &Spot)
{
	reference = Spot;

	Z = normalize(position - reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void Camera3D::Move(const vec3 &Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* Camera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void Camera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}