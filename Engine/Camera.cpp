#include "imgui\imgui.h"
#include "MathGeoLib\src\Geometry\Plane.h"
#include "Globals.h"
#include "Application.h"
#include "Window.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Camera.h"

void Camera::RecalculateFOV()
{
	frustum.SetVerticalFovAndAspectRatio(vertical_fov * DEGTORAD, App->window->GetAspectRatio());
	ResetFrustumPlanes();
}

void Camera::TransformCamera()
{
	frustum.SetFrame(game_object->GetLocalPosition(),	//Pos
		game_object->GetWorldTransform().Col3(2),		//Front
		game_object->GetWorldTransform().Col3(1));		//Up
	ResetFrustumPlanes();
}

void Camera::ResetFrustumPlanes()
{
	planes.clear();
	planes.push_back(frustum.NearPlane());
	planes.push_back(frustum.BottomPlane());
	planes.push_back(frustum.TopPlane());
	planes.push_back(frustum.LeftPlane());
	planes.push_back(frustum.RightPlane());
	planes.push_back(frustum.FarPlane());
}

bool Camera::FrustumCulling(const GameObject* game_object)
{
	if (!game_object->IsCamera() && game_object != App->scene_manager->GetRoot())
	{
		math::vec corner_points[8];
		game_object->GetAABB()->GetCornerPoints(corner_points);

		for (int p = 0; p < 6; ++p)
		{
			uint corners_out = 0;
			for (int i = 0; i < 8; ++i)
			{
				if (corner_points[i].IsFinite())
				{
					if (planes[p].IsOnPositiveSide(corner_points[i]))
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

Camera::Camera(const char* name, bool enabled) : Component(CT_CAMERA, name, enabled), vertical_fov(90), far_plane_dist(500.0f), near_plane_dist(0.5f)
{
}

Camera::~Camera()
{
}

bool Camera::Start()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetType(math::FrustumType::PerspectiveFrustum);
	frustum.SetViewPlaneDistances(near_plane_dist, far_plane_dist);
	TransformCamera();
	RecalculateFOV();

	return true;
}

bool Camera::Update()
{
	if (game_object->transformed)
		TransformCamera();	

	if (frustum_culling)
		FrustumCulling(App->scene_manager->GetRoot());

	return true;
}

void Camera::Inspector()
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::Checkbox("Frustum Culling", &frustum_culling);
		if (ImGui::SliderInt("Field of View", &vertical_fov, 80, 103))
			RecalculateFOV();	

		if (ImGui::SliderFloat("Near Plane Distance", &near_plane_dist, 0.1f, 2.0f))
		{
			frustum.SetNearPlaneDistance(near_plane_dist);
			frustum.ProjectionMatrixChanged();
		}
		if (ImGui::SliderFloat("Far Plane Distance", &far_plane_dist, 0.0f, 1000.0f))
		{
			frustum.SetFarPlaneDistance(far_plane_dist);
			frustum.ProjectionMatrixChanged();
		}

		ImGui::TreePop();
	}
}

bool Camera::IsFrustumActive()
{
	return frustum_culling;
}

void Camera::DrawFrustum()
{
	frustum.Draw(0.0f, 0.0f, 1.0f, 1.0f);
}
