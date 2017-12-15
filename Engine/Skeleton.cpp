#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float4.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "MathGeoLib\src\Math\Quat.h"
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
	if (skeleton != nullptr)
		return skeleton->Inspector();
	LOG("Skeleton Rigg not loaded yet");
	return false;
}

bool Skeleton::IsLoaded() const
{
	return skeleton != nullptr;
}

void Skeleton::Draw(const float3x4& mesh_global_transform) const
{
	if (skeleton != nullptr)
		skeleton->Draw(mesh_global_transform);
	else
		LOG("Skeleton Rigg not loaded yet");
}

void Skeleton::DrawBindPos(const float3x4 & mesh_global_transform) const
{
	skeleton->DrawBindPos(mesh_global_transform);
}

void Skeleton::UpdateJointTransforms(const float3x4 & mesh_world_transform)
{
	if (skeleton != nullptr)
		skeleton->UpdateJointTransforms(mesh_world_transform);
	else
		LOG("Skeleton Rigg not loaded yet");
}

void Skeleton::UpdateJointSpheres()
{
	skeleton->UpdateJointSpheres();
}

Skeleton::Rigg::Rigg(): skeleton_hirarchy(false), selected_joint(nullptr)
{}

Skeleton::Rigg::~Rigg()
{}

bool Skeleton::Rigg::Inspector()
{
	ImGui::Checkbox("Skeleton Hirarchy", &skeleton_hirarchy);

	if (skeleton_hirarchy)
	{
		if(ImGui::Begin("Joint Hirarchy", &skeleton_hirarchy))
			root_joint.Hirarchy(&selected_joint);
		ImGui::End();
	}
		
	return selected_joint->Inspector();
}

void Skeleton::Rigg::Draw(const float3x4& mesh_global_transform) const
{
	root_joint.Draw(mesh_global_transform, selected_joint);
}

void Skeleton::Rigg::DrawBindPos(const float3x4 & mesh_global_transform) const
{
	root_joint.DrawBindPos(mesh_global_transform);
}

void Skeleton::Rigg::UpdateJointTransforms(const float3x4 & mesh_world_transform)
{
	selected_joint->UpdateTransform(mesh_world_transform);
}

void Skeleton::Rigg::UpdateJointSpheres()
{
	root_joint.UpdateSpherePos();
}

Skeleton::Rigg::Joint::Joint(): sphere(vec::zero, JOINT_SPHERE_RADIUS)
{}

void Skeleton::Rigg::Joint::SetTransform(const float3x4 & new_transform)
{
	current_transform = new_transform;

	position = current_transform.TranslatePart();

	float3x3 rotation_matrix = current_transform.RotatePart();
	rotation_matrix.Orthonormalize(0, 1, 2);

	Quat rotation;
	if (rotation_matrix.IsOrthonormal())
		rotation.Set(rotation_matrix);
	else
		rotation.Set(0, 0, 0, 1);

	angles = rotation.ToEulerXYZ();
	angles *= RADTODEG;

	scaling = current_transform.GetScale();
}

bool Skeleton::Rigg::Joint::Inspector()
{
	bool ret = false;

	ImGui::Text("Position:");
	ImGui::PushItemWidth(75.0f);
	if (ImGui::DragFloat("x", &position.x, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("y", &position.y, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("z", &position.z, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;

	ImGui::Text("Rotation:");
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("a", &angles.x, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("b", &angles.y, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("c", &angles.z, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;

	ImGui::Text("Scaling:");
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("i", &scaling.x, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("j", &scaling.y, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	if(ImGui::DragFloat("k", &scaling.z, 1.0f, 0.0f, 0.0f, "%.2f"))
		ret = true;

	return ret;
}

void Skeleton::Rigg::Joint::Hirarchy(Joint ** selected)
{
	bool has_childs = child_joints.size() != 0;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((*selected == this) ? ImGuiTreeNodeFlags_Selected : 0);

	if (!has_childs)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
		node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool node_open = ImGui::TreeNodeEx(this, node_flags, name.c_str());

	if (ImGui::IsItemClicked())
		*selected = this;

	if (node_open)
	{
		for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
			it->Hirarchy(selected);

		if (has_childs)
			ImGui::TreePop();
	}
}

void Skeleton::Rigg::Joint::UpdateSpherePos()
{
	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	pos = current_transform * pos;
	sphere.pos.x = pos.x;
	sphere.pos.y = pos.y;
	sphere.pos.z = pos.z;

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->UpdateSpherePos();
}

void Skeleton::Rigg::Joint::UpdateTransform(const float3x4& mesh_world_transform)
{
	Quat rotation;

	angles *= DEGTORAD;
	rotation = Quat::FromEulerXYX(angles.x, angles.y, angles.z);
	angles *= RADTODEG;

	float3x4 new_transform(float3x4::FromTRS(position, float3x4::FromQuat(rotation), scaling));

	new_transform = mesh_world_transform * new_transform;

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
	{
		float3x4 old_parent_inverse(current_transform);
		if (old_parent_inverse.Inverse() == false)
			LOG("Could not inverse matrix in joint inspector");

		it->UpdateChildTransform(old_parent_inverse, new_transform);
	}

	current_transform = new_transform;
}

void Skeleton::Rigg::Joint::UpdateChildTransform(const float3x4 & old_parent_inverse, const float3x4 & new_parent)
{
	float3x4 local(old_parent_inverse * current_transform);
	current_transform = new_parent * local;

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->UpdateChildTransform(old_parent_inverse, new_parent);
}

void Skeleton::Rigg::Joint::Draw(const float4x4& mesh_global_transform, const Joint* selected) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float4x4 inverse_current_transform(current_transform);
	if (inverse_current_transform.Inverse() == false)
		LOG("Could not inverse transform in joint draw");

	float4x4 transform(mesh_global_transform * inverse_current_transform);

	pos = transform * pos;
	
	x = transform * x;
	vec x_vec(x.xyz() - pos.xyz());
	x_vec = x_vec / x_vec.Length() * JOINT_SPHERE_RADIUS;
	x_vec = pos.xyz() + x_vec;

	y = transform * y;
	vec y_vec(y.xyz() - pos.xyz());
	y_vec = y_vec / y_vec.Length() * JOINT_SPHERE_RADIUS;
	y_vec = pos.xyz() + y_vec;

	z = transform * z;
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

		float4x4 inverse_child(it->current_transform);
		if (inverse_child.Inverse() == false)
			LOG("Could not inverse transform in joint draw");

		float4x4 child_world_transform(mesh_global_transform * inverse_child);
		child_vec = child_world_transform * child_vec;

		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(child_vec.x, child_vec.y, child_vec.z);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Sphere draw in world pos
	Sphere world_sphere(pos.xyz(), JOINT_SPHERE_RADIUS);

	if (this == selected)
		world_sphere.Draw(1.0f, 0.65f, 1.0f, 1.0f);
	else
		world_sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->Draw(mesh_global_transform, selected);
}

void Skeleton::Rigg::Joint::DrawBindPos(const float4x4 & mesh_global_transform) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float4x4 transform(mesh_global_transform * inverse_bind_pose_transform);

	pos = transform * pos;

	x = transform * x;
	vec x_vec(x.xyz() - pos.xyz());
	x_vec = x_vec / x_vec.Length() * JOINT_SPHERE_RADIUS;
	x_vec = pos.xyz() + x_vec;

	y = transform * y;
	vec y_vec(y.xyz() - pos.xyz());
	y_vec = y_vec / y_vec.Length() * JOINT_SPHERE_RADIUS;
	y_vec = pos.xyz() + y_vec;

	z = transform * z;
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
		float4x4 child_world_transform(mesh_global_transform * it->inverse_bind_pose_transform);
		child_vec = child_world_transform * child_vec;

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
