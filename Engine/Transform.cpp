#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\TransformOps.h"
#include "imgui/imgui.h"
#include "Globals.h"
#include "Transform.h"

Transform::Transform(const char * name) : name(name), pitch(0), roll(0), yaw(0), translation(0, 0, 0), scaling(1, 1, 1), rotation(0, 0, 0, 1), center(0, 0, 0)
{
	transform_matrix = transform_matrix.identity;
}

Transform::~Transform()
{}

void Transform::SetTransform(const math::float4x4& new_transform)
{
	transform_matrix = new_transform;

	translation = transform_matrix.Transposed().TranslatePart();
	float3x3 rotation_matrix = transform_matrix.RotatePart();
	rotation_matrix.Orthonormalize(0, 1, 2);
	rotation = rotation_matrix.ToQuat();
	scaling = transform_matrix.GetScale();	
}

void Transform::SetTransformCenter(const math::vec center_)
{
	center = center_;
}

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

void Transform::MaintainScalePositive()
{
	if (scaling.x < MIN_SCALE)	scaling.x = MIN_SCALE;
	if (scaling.y < MIN_SCALE)	scaling.y = MIN_SCALE;
	if (scaling.z < MIN_SCALE)	scaling.z = MIN_SCALE;
}

const float4x4& Transform::GetTransformMatrix()
{
	return transform_matrix;
}

const vec& Transform::GetTransformTranslation()
{
	return translation;
}

const vec& Transform::GetTransformScale()
{
	return scaling;
}

const Quat& Transform::GetTransformRotation()
{
	return rotation;
}

const vec & Transform::GetTransformRotationAngles()
{
	return vec(roll, pitch, yaw);
}

Quat Transform::TransformEuler2Quat(const vec euler)
{
	Quat ret;
	Euler2Quat(euler.x, euler.y, euler.z, ret);
	return ret;
}

void Transform::Update()
{
	transform_matrix.SetRow(3, float4(translation.x, translation.y, translation.z, 1));	//Translate
	float3x3 rotation_matrix(rotation);
	transform_matrix.Set3x3Part(rotation_matrix);										//Rotate
	transform_matrix = float4x4::Scale(scaling, vec(0, 0, 0)) * transform_matrix;		//Scalate
}

bool Transform::Inspector()
{
	bool ret = false;
	if (ImGui::TreeNode("Transform"))
	{
		if (ImGui::DragFloat("Pos x", &translation.x))	ret = true;
		if (ImGui::DragFloat("Pos y", &translation.y))	ret = true;
		if (ImGui::DragFloat("Pos z", &translation.z))	ret = true;

		ImGui::Separator();

		Quat2Euler(rotation, roll, pitch, yaw);

		roll *= RADTODEG;
		pitch *= RADTODEG;
		yaw *= RADTODEG;

		//It's the same, but visually better for the user
		if (roll == -180)	roll = 180;
		if (pitch == -180)	pitch = 180;
		if (yaw == -180)	yaw = 180;

		if (ImGui::DragFloat("Rotation x", &yaw))	ret = true;
		if (ImGui::DragFloat("Rotation y", &pitch))	ret = true;
		if (ImGui::DragFloat("Rotation z", &roll))	ret = true;
		
		roll *= DEGTORAD;
		pitch *= DEGTORAD;
		yaw *= DEGTORAD;

		Euler2Quat(roll, pitch, yaw, rotation);

		ImGui::Separator();

		if (ImGui::DragFloat("Scale x", &scaling.x, 0.1f, MIN_SCALE))	ret = true;
		if (ImGui::DragFloat("Scale y", &scaling.y, 0.1f, MIN_SCALE))	ret = true;
		if (ImGui::DragFloat("Scale z", &scaling.z, 0.1f, MIN_SCALE))	ret = true;

		MaintainScalePositive();
		
		ImGui::TreePop();
	}
	return ret;
}