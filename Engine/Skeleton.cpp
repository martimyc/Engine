#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float4.h"
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

void Skeleton::Draw(const float4x4& mesh_global_transform) const
{
	if (skeleton != nullptr)
		skeleton->Draw(mesh_global_transform);
}

void Skeleton::DrawBindPos(const float4x4 & mesh_global_transform) const
{
	skeleton->DrawBindPos(mesh_global_transform);
}

void Skeleton::UpdateJointSpheres()
{
	skeleton->UpdateJointSpheres();
}

void Skeleton::Rigg::Draw(const float4x4& mesh_global_transform) const
{
	root_joint.Draw(mesh_global_transform);
}

void Skeleton::Rigg::DrawBindPos(const float4x4 & mesh_global_transform) const
{
	root_joint.DrawBindPos(mesh_global_transform);
}

void Skeleton::Rigg::UpdateJointSpheres()
{
	root_joint.UpdateSpherePos();
}

Skeleton::Rigg::Joint::Joint(): sphere(vec::zero, JOINT_SPHERE_RADIUS)
{}

void Skeleton::Rigg::Joint::UpdateSpherePos()
{
	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	pos = pos * current_inverse_transform;
	sphere.pos.x = pos.x;
	sphere.pos.y = pos.y;
	sphere.pos.z = pos.z;

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->UpdateSpherePos();
}

void Skeleton::Rigg::Joint::Draw(const float4x4& mesh_global_transform) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float4x4 transform(current_inverse_transform * mesh_global_transform);

	pos = pos * transform;
	x = x * transform;
	y = y * transform;
	z = z * transform;

	//X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(x.x, x.y, x.z);

	//Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(y.x, y.y, y.z);

	//Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(z.x, z.y, z.z);

	//Bones
	glColor4f(1.0f, 0.65f, 0.0f, 1.0f);

	float4x4 child_transform;
	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
	{
		float4 child_vec(0.0f, 0.0f, 0.0f, 1.0f);
		float4x4 child_world_transform(it->current_inverse_transform * mesh_global_transform);
		child_vec = child_vec * child_world_transform;

		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(child_vec.x, child_vec.y, child_vec.z);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Sphere draw in world pos
	Sphere world_sphere(pos.xyz(), JOINT_SPHERE_RADIUS);
	world_sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->Draw(mesh_global_transform);
}

void Skeleton::Rigg::Joint::DrawBindPos(const float4x4 & mesh_global_transform) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float4x4 transform(inverse_bind_pose_transform * mesh_global_transform);

	pos = pos * transform;

	x = x * transform;
	vec x_vec(x.xyz() - pos.xyz());
	x_vec = x_vec / x_vec.Length() * JOINT_SPHERE_RADIUS;
	x_vec = pos.xyz() + x_vec;

	y = y * transform;
	vec y_vec(y.xyz() - pos.xyz());
	y_vec = y_vec / y_vec.Length() * JOINT_SPHERE_RADIUS;
	y_vec = pos.xyz() + y_vec;

	z = z * transform;
	vec z_vec(z.xyz() - pos.xyz());
	z_vec = z_vec / z_vec.Length() * JOINT_SPHERE_RADIUS;
	z_vec = pos.xyz() + z_vec;

	//X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(x_vec.x, x_vec.y, x_vec.z);

	//Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(y_vec.x, y_vec.y, y_vec.z);

	//Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(z_vec.x, z_vec.y, z_vec.z);

	//Bones
	glColor4f(1.0f, 0.65f, 0.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
	{
		float4 child_vec(0.0f, 0.0f, 0.0f, 1.0f);
		float4x4 child_world_transform(it->inverse_bind_pose_transform * mesh_global_transform);
		child_vec = child_vec * child_world_transform;

		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(child_vec.x, child_vec.y, child_vec.z);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Sphere draw in world pos
	Sphere world_sphere(pos.xyz(), JOINT_SPHERE_RADIUS);
	world_sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->DrawBindPos(mesh_global_transform);
}
