#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "Skeleton.h"

Skeleton::Skeleton(const std::string & name, const UID & uid) : Resource(RT_SKELETON, name, uid), skeleton(nullptr)
{}

Skeleton::Skeleton(const std::string & name, Rigg * source) : Resource(RT_SKELETON, name), skeleton(source)
{}

Skeleton::~Skeleton()
{
	delete skeleton;
}

void Skeleton::SetRigg(Rigg* new_rigg)
{
	skeleton = new_rigg;
}

bool Skeleton::Inspector()
{
	return false;
}

bool Skeleton::IsLoaded() const
{
	return skeleton != nullptr;
}

void Skeleton::Draw(const GLfloat* opengl_view_matrix) const
{
	if (skeleton != nullptr)
		skeleton->Draw(opengl_view_matrix);
}

void Skeleton::Rigg::Draw(const GLfloat* opengl_view_matrix) const
{
	/*glPushMatrix();
	glLoadMatrixf(opengl_view_matrix);*/

	//float4x4 transform = skeleton_transform;

	//glMultMatrixf(&transform.At(0, 0));

	root_joint.Draw(skeleton_transform);

	//glPopMatrix();
}

Skeleton::Rigg::Joint::Joint(): sphere(vec::zero, JOINT_SPHERE_RADIUS)
{}

void Skeleton::Rigg::Joint::Draw(const float4x4& skeleton_transform) const
{
	float4x4 transform = inverse_bind_pose_transform; //Should inverse?
	transform.Inverse();
	transform = skeleton_transform * transform;

	//Sphere
	//sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	glBegin(GL_LINES);

	/*//X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);

	//Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);

	//Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);*/

	//Bones
	glColor4f(1.0f, 0.65f, 0.0f, 1.0f);

	float4x4 child_transform;
	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
	{
		child_transform = it->inverse_bind_pose_transform;
		child_transform.Inverse();
		child_transform = child_transform * transform;

		glVertex3f(transform[2][0], transform[2][1], transform[2][2]);
		glVertex3f(child_transform[2][0], child_transform[2][1], child_transform[2][2]);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->Draw(skeleton_transform);
}
