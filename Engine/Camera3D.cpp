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

Camera3D::Camera3D(const char* name, bool start_enabled) : Module(name, start_enabled),
near_plane_dist_editor_camera(0.5f),
far_plane_dist_editor_camera(500.0f),
pos(0.0f, 1.0f, 3.0f),
vertical_fov_editor_camera(80)
{}

Camera3D::~Camera3D()
{}

bool Camera3D::Init()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());

	editor_camera_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	editor_camera_frustum.SetType(math::FrustumType::PerspectiveFrustum);
	editor_camera_frustum.SetViewPlaneDistances(near_plane_dist_editor_camera, far_plane_dist_editor_camera);
	editor_camera_frustum.SetFrame(pos, vec(0, 0, -1), vec(0, 1, 0));
	RecalculateFOV();

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

	if (App->BeginDockWindow("Camera 3D", &config_camera))
	{
		const GameObject* focused_game_object = App->scene_manager->GetFocused();

		if (focused_game_object != nullptr)
		{
			if (focused_game_object->GetNumComponents() <= 1)
			{
				ImGui::SliderFloat("WASD speed", &camera_speed, 2.0f, 15.0f);
				ImGui::SliderFloat("Zoom speed", &camera_zoom_speed, 0.5f, 5.0f);
				ImGui::SliderFloat("Rotation sensivility", &sensibility, 0.1f, 0.5f);

				//ImGui::SliderFloat("Near Plane Distance", &near_plane_dist_editor_camera, 0.1f, 0.5f);
				ImGui::SliderFloat("Far Plane Distance", &far_plane_dist_editor_camera, 250.0f, 1000.0f);
				ImGui::SliderFloat("Field of View", &vertical_fov_editor_camera, 60.0f, 103.0f);

				if (ImGui::Button("Apply"))
				{
					//editor_camera_frustum.SetNearPlaneDistance(near_plane_dist_editor_camera);
					editor_camera_frustum.SetFarPlaneDistance(far_plane_dist_editor_camera);
					RecalculateFOV();
				}
			}
		}
	}
	App->EndDockWindow();

	if (show_matrix_debug)
	{
		ImGui::Begin("Matrix Debug", &show_matrix_debug);
		ShowMatrixDebug();
		ImGui::End();
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

	bool camera_moved = false;
	float speed = camera_speed * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = camera_speed * 2 * dt;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)	//Frontwards
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos + editor_camera_frustum.front * speed);
			camera_moved = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)	//Backwards
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos - editor_camera_frustum.front * speed); 
			camera_moved = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)	//Left
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos - math::Cross(editor_camera_frustum.front, editor_camera_frustum.up) * speed); 
			camera_moved = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)	//Right
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos + math::Cross(editor_camera_frustum.front, editor_camera_frustum.up) * speed); 
			camera_moved = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)	//Up
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos + editor_camera_frustum.up * speed); 
			camera_moved = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)	//Bottom
		{
			editor_camera_frustum.SetPos(editor_camera_frustum.pos - editor_camera_frustum.up * speed);
			camera_moved = true;
		}
	}
	
	// Mouse whell motion:	-1 equals down, 1 equals up
	if (App->input->GetMouseZ() == -1)
	{
		editor_camera_frustum.SetPos(editor_camera_frustum.pos - editor_camera_frustum.front * camera_zoom_speed); 
		camera_moved = true;
	}
	else if (App->input->GetMouseZ() == 1)
	{
		editor_camera_frustum.SetPos(editor_camera_frustum.pos + editor_camera_frustum.front * camera_zoom_speed);
		camera_moved = true;
	}

	// Center game object
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		const GameObject* focused_game_object = App->scene_manager->GetFocused();
		if (focused_game_object != nullptr)
		{
			CenterToGameObject(focused_game_object);
			camera_moved = true;
		}
	}

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		if(dx != 0)
		{
			vec temp_pos = editor_camera_frustum.pos;
			float delta_x = (float)dx * sensibility;
			rotation.SetFromAxisAngle(vec(0, 1, 0), delta_x * DEGTORAD);
			editor_camera_frustum.TransformInverted(rotation);
			editor_camera_frustum.pos = temp_pos;
			editor_camera_frustum.WorldMatrixChanged();
			camera_moved = true;
		}

		if(dy != 0)
		{
			vec temp_pos = editor_camera_frustum.pos;
			float delta_y = (float)dy * sensibility;
			rotation.SetFromAxisAngle(math::Cross(editor_camera_frustum.front, vec(0, 1, 0)), delta_y * DEGTORAD);
			editor_camera_frustum.TransformInverted(rotation);
			editor_camera_frustum.pos = temp_pos;
			editor_camera_frustum.WorldMatrixChanged();
			camera_moved = true;
		}
	}

	if (camera_moved)
		ResetFrustumPlanes();

	return UPDATE_CONTINUE;
}

void Camera3D::LookAt(const vec &spot, float distance)
{
	editor_camera_frustum.SetPos(spot - (editor_camera_frustum.front * distance));
}

void Camera3D::RecalculateFOV()
{
	editor_camera_frustum.SetVerticalFovAndAspectRatio(vertical_fov_editor_camera * DEGTORAD, App->window->GetAspectRatio());
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glLoadMatrixf(App->camera->GetProjMatrix()); 
	ResetFrustumPlanes();
}

void Camera3D::ResetFrustumPlanes()
{
	frustum_planes.clear();
	for (int i = 0; i < 6; i++)
		frustum_planes.push_back(editor_camera_frustum.GetPlane(i));
}

void Camera3D::ShowMatrixDebug()
{
	if ((ImGui::InputFloat("Camera position X", &editor_camera_frustum.pos.x))
		|| (ImGui::InputFloat("Camera position Y", &editor_camera_frustum.pos.y))
		|| (ImGui::InputFloat("Camera position Z", &editor_camera_frustum.pos.z)))
	{
		editor_camera_frustum.SetPos(editor_camera_frustum.pos);
	}

	ImGui::Separator();

	ImGui::Text("Camera Up    Vector:\t(%.4f, %.4f, %.4f)",
		editor_camera_frustum.up.x,
		editor_camera_frustum.up.y,
		editor_camera_frustum.up.z);

	ImGui::Text("Camera Front Vector:\t(%.4f, %.4f, %.4f)",
		editor_camera_frustum.front.x,
		editor_camera_frustum.front.y,
		editor_camera_frustum.front.z);

	vec left_frustum_vec = math::Cross(editor_camera_frustum.up, editor_camera_frustum.front);
	ImGui::Text("Camera Left  Vector:\t(%.4f, %.4f, %.4f)",
		left_frustum_vec.x,
		left_frustum_vec.y,
		left_frustum_vec.z);

	ImGui::Separator();
	ImGui::Separator();

	ImGui::Text("Frustum Projection Matrix:");
	float* proj_mat = editor_camera_frustum.ProjectionMatrix().ptr();
	ImGui::Columns(4, "proj_mat", false);
	ImGui::Separator();
	for (int i = 0; i < 16; i++)
	{
		ImGui::Text("%.3f", proj_mat[i]);
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::Text("Frustum View Matrix:");
	float* view_mat = editor_camera_frustum.ViewMatrix().ptr();
	ImGui::Columns(4, "view_mat", false);
	ImGui::Separator();
	for (int i = 0; i < 16; i++)
	{
		ImGui::Text("%.3f", view_mat[i]);
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::Text("Frustum View Projection Matrix:");
	float* view_proj_mat = editor_camera_frustum.ViewProjMatrix().ptr();
	ImGui::Columns(4, "view_proj_mat", false);
	ImGui::Separator();
	for (int i = 0; i < 16; i++)
	{
		ImGui::Text("%.3f", view_proj_mat[i]);
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	ImGui::Separator();
}

// -----------------------------------------------------------------

void Camera3D::OpenCloseConfigCameraWindow()
{
	config_camera = !config_camera;
}

const float Camera3D::GetFovVertical() const
{
	return editor_camera_frustum.VerticalFov();
}

const float * Camera3D::GetProjMatrix() const
{
	return editor_camera_frustum.ProjectionMatrix().ptr();
}

const float * Camera3D::GetProjMatrixTransposed() const
{
	return editor_camera_frustum.ProjectionMatrix().Transposed().ptr();
}

const float * Camera3D::GetViewProjMatrix() const
{
	return editor_camera_frustum.ViewProjMatrix().ptr();
}

const float * Camera3D::GetViewProjMatrixTransposed() const
{
	return editor_camera_frustum.ViewProjMatrix().Transposed().ptr();
}

const float * Camera3D::GetViewMatrix() const
{
	static float4x4 matrix;
	matrix = editor_camera_frustum.ViewMatrix();
	matrix.Transpose();
	return (float*)matrix.ptr();
}

const float * Camera3D::GetWorldMatrix() const
{
	return editor_camera_frustum.WorldMatrix().ptr();
}

void Camera3D::CenterToGameObject(const GameObject* game_object)
{
	vec center = game_object->GetWorldPosition();
	float distance = game_object->GetAABB()->Diagonal().Length();
	if (distance == inf)
		distance = 2.0f;	//Standard distance if the object has no AABB (i.e. camera)
	LookAt(center, distance);
}

void Camera3D::OpenCloseMatricesDebugWindow()
{
	show_matrix_debug = !show_matrix_debug;
}

bool Camera3D::DoFrustumCulling(const GameObject * game_obj)
{
	editor_camera_frustum.Draw(1, 0, 0, 1);
	if (!game_obj->IsCamera() && game_obj != App->scene_manager->GetRoot())
	{
		math::vec corner_points[8];
		game_obj->GetAABB()->GetCornerPoints(corner_points);

		for (int p = 0; p < 6; ++p)
		{
			uint corners_out = 0;
			for (int i = 0; i < 8; ++i)
			{
				if (corner_points[i].IsFinite())
				{
					if (frustum_planes[p].IsOnPositiveSide(corner_points[i]))
					{
						corners_out++;
					}
				}
			}
			if (corners_out == 8)
			{
				//Is out
				//Dont draw
				return false;
			}

		}
		return true;
	}
}
