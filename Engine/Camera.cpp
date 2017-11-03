#include "imgui\imgui.h"
#include "Camera.h"

Camera::Camera(const char* name, bool enabled) : Component(CT_CAMERA, name, enabled)
{
	frustum.SetType(math::PerspectiveFrustum);
	frustum.SetNearPlaneDistance(1.0f);
	frustum.SetFarPlaneDistance(20.0f);
	
}

Camera::~Camera()
{
}

bool Camera::Update()
{

	return true;
}

void Camera::Inspector()
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::Checkbox("Frustum Culling", &frustum_culling);

		ImGui::TreePop();
	}
}
