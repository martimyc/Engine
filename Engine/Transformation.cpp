#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "imgui/imgui.h"
#include "Globals.h"
#include "Transformation.h"

void Transform::Quat2Euler(const Quat q, float & roll, float & pitch, float & yaw)
{
	double sinr = +2.0 * (q.x * q.y + q.z * q.w);
	double cosr = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	roll = atan2(sinr, cosr);

	double sinp = +2.0 * (q.x * q.z - q.w * q.y);
	if (fabs(sinp) >= 1)
		pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	double siny = +2.0 * (q.x * q.w + q.y * q.z);
	double cosy = +1.0 - 2.0 * (q.z * q.z + q.w * q.w);
	yaw = atan2(siny, cosy);
}

void Transform::Euler2Quat(const float roll, const float pitch, const float yaw, Quat& q)
{
	// Abbreviations for the various angular functions, so we have to calculate the less cos/sin as possible
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);

	q.x = cy * cr * cp + sy * sr * sp;
	q.y = cy * sr * cp - sy * cr * sp;
	q.z = cy * cr * sp + sy * sr * cp;
	q.w = sy * cr * cp - cy * sr * sp;
}

Transform::Transform(const char * const name, GameObject* game_object, bool enabled) :Component(CT_TRANSFORMATION, name, game_object, enabled), pitch(0), roll(0), yaw(0), translation(0, 0, 0), scaling(1, 1, 1), rotation(0, 0, 0, 1)
{
	transform_matrix = transform_matrix.identity;
}

Transform::~Transform()
{
}

const float * Transform::GetTransformMatrix()
{
	return &transform_matrix.At(0, 0);
}

void Transform::Inspector(int num_component)
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::InputFloat("Pos x", &translation.x, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Pos y", &translation.y, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Pos z", &translation.z, 0.0f, 0.0f, 2);

		ImGui::Separator();

		Quat2Euler(rotation, roll, pitch, yaw);

		roll *= RADTODEG;
		pitch *= RADTODEG;
		yaw *= RADTODEG;

		ImGui::InputFloat("Rotation x", &roll, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Rotation y", &pitch, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Rotation z", &yaw, 0.0f, 0.0f, 2);
		
		roll *= DEGTORAD;
		pitch *= DEGTORAD;
		yaw *= DEGTORAD;

		Euler2Quat(roll, pitch, yaw, rotation);

		ImGui::Separator();

		ImGui::InputFloat("Scale x", &scaling.x, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Scale y", &scaling.y, 0.0f, 0.0f, 2);
		ImGui::InputFloat("Scale z", &scaling.z, 0.0f, 0.0f, 2);

		transform_matrix.SetRow(3, float4(translation.x, translation.y, translation.z, 1));
		
		float3x3 rotation_matrix(rotation);
			/*
1.0f - 2.0f*rotation.y*rotation.y - 2.0f*rotation.z*rotation.z, 2.0f*rotation.x*rotation.y - 2.0f*rotation.z*rotation.w, 2.0f*rotation.x*rotation.z + 2.0f*rotation.y*rotation.w,
2.0f*rotation.x*rotation.y + 2.0f*rotation.z*rotation.w, 1.0f - 2.0f*rotation.x*rotation.x - 2.0f*rotation.z*rotation.z, 2.0f*rotation.y*rotation.z - 2.0f*rotation.x*rotation.w,
2.0f*rotation.x*rotation.z - 2.0f*rotation.y*rotation.w, 2.0f*rotation.y*rotation.z + 2.0f*rotation.x*rotation.w, 1.0f - 2.0f*rotation.x*rotation.x - 2.0f*rotation.y*rotation.y,
0.0f, 0.0f, 0.0f);
			*/
		transform_matrix.Set3x3Part(rotation_matrix);

		transform_matrix[0][0] *= scaling.x;
		transform_matrix[1][1] *= scaling.y;
		transform_matrix[2][2] *= scaling.z;

		ImGui::TreePop();
	}
}