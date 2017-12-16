#include "SDL2\include\SDL_opengl.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\TransformOps.h"
#include "imgui/imgui.h"
#include "Globals.h"
#include "Transform.h"

Transform::Transform(const char * name) : name(name), translation(0, 0, 0), scaling(1, 1, 1), rotation(0, 0, 0, 1), euler_rotation(0, 0, 0)
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
	if (rotation_matrix.IsOrthonormal())
		rotation.Set(rotation_matrix);
	else
		rotation.Set(0, 0, 0, 1);
	euler_rotation = rotation.ToEulerXYZ();
	euler_rotation *= RADTODEG;
	scaling = transform_matrix.GetScale();
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
	return euler_rotation;
}

Quat Transform::TransformEuler2Quat(const vec euler)
{
	Quat ret;
	Euler2Quat(euler.x, euler.y, euler.z, ret);
	return ret;
}

void Transform::Update()
{
	euler_rotation *= DEGTORAD;

	rotation = Quat::FromEulerXYZ(euler_rotation.x, euler_rotation.y, euler_rotation.z);

	transform_matrix = float4x4::FromQuat(rotation);									//Rotate
	euler_rotation *= RADTODEG;

	transform_matrix = float4x4::Scale(scaling, vec(0, 0, 0)) * transform_matrix;		//Scalate
	transform_matrix.SetRow(3, float4(translation.x, translation.y, translation.z, 1));	//Translate
}

bool Transform::Inspector()
{
	bool ret = false;
	if (ImGui::TreeNode("Transform"))
	{
		if (ImGui::DragFloat("Pos x", &translation.x, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Pos y", &translation.y, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Pos z", &translation.z, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;

		ImGui::Separator();

		if (ImGui::DragFloat("Rotation x", &euler_rotation.x, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Rotation y", &euler_rotation.y, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Rotation z", &euler_rotation.z, 1.0f, 0.0f, 0.0f, "%.2f"))	ret = true;

		ImGui::Separator();

		if (ImGui::DragFloat("Scale x", &scaling.x, 0.1f, MIN_SCALE, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Scale y", &scaling.y, 0.1f, MIN_SCALE, 0.0f, "%.2f"))	ret = true;
		if (ImGui::DragFloat("Scale z", &scaling.z, 0.1f, MIN_SCALE, 0.0f, "%.2f"))	ret = true;

		MaintainScalePositive();

		ImGui::TreePop();
	}
	return ret;
}