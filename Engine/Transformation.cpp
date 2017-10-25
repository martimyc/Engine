#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\TransformOps.h"
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
		bool translate = false;
		bool rotate = false;
		bool scalate = false;

		if (ImGui::DragFloat("Pos x", &translation.x))	translate = true;
		if (ImGui::DragFloat("Pos y", &translation.y))	translate = true;
		if (ImGui::DragFloat("Pos z", &translation.z))	translate = true;

		ImGui::Separator();

		Quat2Euler(rotation, roll, pitch, yaw);

		roll *= RADTODEG;
		pitch *= RADTODEG;
		yaw *= RADTODEG;

		//It's the same, but visually better for the user
		if (roll == -180)	roll = 180;
		if (pitch == -180)	pitch = 180;
		if (yaw == -180)	yaw = 180;

		if (ImGui::DragFloat("Rotation x", &yaw))	rotate = true;
		if (ImGui::DragFloat("Rotation y", &pitch))	rotate = true;
		if (ImGui::DragFloat("Rotation z", &roll))	rotate = true;
		
		roll *= DEGTORAD;
		pitch *= DEGTORAD;
		yaw *= DEGTORAD;

		Euler2Quat(roll, pitch, yaw, rotation);

		ImGui::Separator();

		if (ImGui::DragFloat("Scale x", &scaling.x, 0.1f))	scalate = true;
		if (ImGui::DragFloat("Scale y", &scaling.y, 0.1f))	scalate = true;
		if (ImGui::DragFloat("Scale z", &scaling.z, 0.1f))	scalate = true;


		if (translate || rotate || scalate)
		{
			transform_matrix.SetRow(3, float4(translation.x, translation.y, translation.z, 1));	//Translate
			float3x3 rotation_matrix(rotation);
			transform_matrix.Set3x3Part(rotation_matrix);										//Rotate
			transform_matrix = float4x4::Scale(scaling, float3(0, 0, 0)) * transform_matrix;	//Scalate
		}

		ImGui::TreePop();
	}
}