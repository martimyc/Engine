#include "SDL2\include\SDL_opengl.h"
#include "Assimp\include\matrix4x4.inl"
#include "imgui/imgui.h"
#include "Globals.h"
#include "Transformation.h"

void Transform::Quat2Euler(const aiQuaternion q, float & roll, float & pitch, float & yaw)
{
	double sinr = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	roll = atan2(sinr, cosr);

	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	double siny = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	yaw = atan2(siny, cosy);
}

void Transform::Euler2Quat(const float roll, const float pitch, const float yaw, aiQuaternion& q)
{
	// Abbreviations for the various angular functions, so we have to calculate the less cos/sin as possible
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);

	q.w = cy * cr * cp + sy * sr * sp;
	q.x = cy * sr * cp - sy * cr * sp;
	q.y = cy * cr * sp + sy * sr * cp;
	q.z = sy * cr * cp - cy * sr * sp;
}

Transform::Transform(const char * const name, bool enabled) :Component(CT_TRANSFORMATION, name, enabled), pitch(0), roll(0), yaw(0), translation(0, 0, 0), scaling(1, 1, 1), rotation(1, 0, 0, 0)
{
	matrix = new aiMatrix4x4();
}

Transform::~Transform()
{
	DELETE_PTR(matrix)
}

void Transform::TranslateRotateScalate()
{
	const GLfloat* GLmatrix = &matrix->a1;
	glPushMatrix();
	glLoadMatrixf(GLmatrix);
}

void Transform::ResetTranslateRotateScalate()
{
	glPopMatrix();
}

void Transform::Inspector(int num_component)
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::InputFloat("Pos x", &translation.x);
		ImGui::InputFloat("Pos y", &translation.y);
		ImGui::InputFloat("Pos z", &translation.z);

		ImGui::Separator();

		Quat2Euler(rotation, roll, pitch, yaw);

		roll *= RADTODEG;
		pitch *= RADTODEG;
		yaw *= RADTODEG;

		ImGui::InputFloat("Rotation x", &roll);
		ImGui::InputFloat("Rotation y", &pitch);
		ImGui::InputFloat("Rotation z", &yaw);
		
		roll *= DEGTORAD;
		pitch *= DEGTORAD;
		yaw *= DEGTORAD;

		Euler2Quat(roll, pitch, yaw, rotation);

		ImGui::Separator();

		aiVector3D scaling_temp = scaling;
		ImGui::InputFloat("Scale x", &scaling.x);
		ImGui::InputFloat("Scale y", &scaling.y);
		ImGui::InputFloat("Scale z", &scaling.z);

		matrix->a4 = translation.x;
		matrix->b4 = translation.y;
		matrix->c4 = translation.z;

		//TODO Rotate/scalate
		/*
		if (scaling_temp != scaling)
		{
			matrix->a1 *= scaling.x;
			matrix->b2 *= scaling.y;
			matrix->c3 *= scaling.z;
		}
		*/
		ImGui::TreePop();
	}
}