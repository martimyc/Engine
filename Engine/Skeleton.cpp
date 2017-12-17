#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float4.h"
#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "Animation.h"
#include "Mesh.h"
#include "Skeleton.h"

Skeleton::Skeleton(const std::string & name, const UID & uid) : Resource(RT_SKELETON, name, uid), skeleton(nullptr), deformable_vertices(nullptr)
{}

Skeleton::Skeleton(const std::string & name, Rigg * source) : Resource(RT_SKELETON, name), skeleton(source), deformable_vertices(nullptr)
{}

Skeleton::~Skeleton()
{
	delete skeleton;
	delete[] deformable_vertices;
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

void Skeleton::UnLoad()
{
	delete skeleton;
	skeleton = nullptr;
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

void Skeleton::SetWorldPositions(const float3x4 & mesh_world_transform)
{
	if (skeleton != nullptr)
		skeleton->SetWorldPositions(mesh_world_transform);
	else
		LOG("Skeleton Rigg not loaded yet");
}

void Skeleton::ChangeJointTransforms(Animation* anim, double anim_time, bool interpolation)
{
	if (skeleton != nullptr)
		skeleton->ChangeJointTransforms(anim, anim_time, interpolation);
	else
		LOG("Skeleton Rigg not loaded yet");
}

unsigned int Skeleton::GetNumJoints() const
{
	if (skeleton != nullptr)
		skeleton->GetNumJoints();
	LOG("Skeleton Rigg not loaded yet");
	return 0;
}

void Skeleton::DeformableMesh(const Mesh * mesh)
{
	deformable_vertices = new GLfloat[mesh->GetNumVertices() * 3];
}

void Skeleton::UpdateMesh(const Mesh* original)
{
	skeleton->GetVertices(original, deformable_vertices);

	if (deformable_vertices != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, original->GetVerticesID());
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * original->GetNumVertices() * 3,
			deformable_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, original->GetNormalsID());
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * original->GetNumVertices() * 3,
			deformable_vertices, GL_DYNAMIC_DRAW); //pdf said vertices here
	}
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
	root_joint.Draw(mesh_global_transform * transform, selected_joint);
}

void Skeleton::Rigg::DrawBindPos(const float3x4 & mesh_global_transform) const
{
	root_joint.DrawBindPos(mesh_global_transform);
}

void Skeleton::Rigg::UpdateJointTransforms(const float3x4 & mesh_world_transform)
{
	root_joint.UpdateTransforms(mesh_world_transform * transform, selected_joint);
	root_joint.SetWorldPositions(mesh_world_transform * transform);
}

void Skeleton::Rigg::SetWorldPositions(const float3x4 & mesh_world_transform)
{
	root_joint.SetWorldPositions(mesh_world_transform * transform);
}

void Skeleton::Rigg::ChangeJointTransforms(Animation* anim, double anim_time, bool interpolation)
{
	root_joint.ChangeTransforms(anim, anim_time, interpolation);
}

unsigned int Skeleton::Rigg::GetNumJoints() const
{
	return num_joints;
}

void Skeleton::Rigg::GetVertices(const Mesh * original, GLfloat * vertices)
{
	memset(vertices, 0.0f, original->GetNumVertices() * 3 * sizeof(GLfloat));

	root_joint.GetVertices(original, vertices, transform);
}

Skeleton::Rigg::Joint::Joint(): sphere(vec::zero, JOINT_SPHERE_RADIUS)
{}

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

void Skeleton::Rigg::Joint::UpdateTransforms(const float3x4& parent_global, const Joint* selected)
{
	if (selected == this)
	{
		angles *= DEGTORAD;

		float3x4 new_world_transform(float3x4::FromTRS(position, float3x4::FromEulerXYZ(angles.x, angles.y, angles.z), scaling));

		angles *= RADTODEG;

		current_transform = parent_global.Inverted() * new_world_transform;
	}
	
	float3x4 global(parent_global * current_transform);

	//ImGui
	position = global.TranslatePart();

	float3x3 rotation_matrix = global.RotatePart();

	rotation_matrix.Orthonormalize(0, 1, 2);

	angles = rotation_matrix.ToEulerXYZ();
	angles *= RADTODEG;

	float* test = &angles.x;

	scaling = global.GetScale();
	//---

	//Sphere
	sphere.pos.x = position.x;
	sphere.pos.y = position.y;
	sphere.pos.z = position.z;
	//---

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->UpdateTransforms(global, selected);
}

void Skeleton::Rigg::Joint::Draw(const float3x4& global_transform, const Joint* selected) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float3x4 transform(global_transform * current_transform);

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

		float3x4 child_world_transform(transform * it->current_transform);
		child_vec = child_world_transform * child_vec;

		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(child_vec.x, child_vec.y, child_vec.z);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Sphere
	if (this == selected)
		sphere.Draw(1.0f, 0.65f, 1.0f, 1.0f);
	else
		sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->Draw(transform, selected);
}

void Skeleton::Rigg::Joint::DrawBindPos(const float3x4 & mesh_global_transform) const
{
	glBegin(GL_LINES);

	float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	float4 x(1.0f, 0.0f, 0.0f, 1.0f);
	float4 y(0.0f, 1.0f, 0.0f, 1.0f);
	float4 z(0.0f, 0.0f, 1.0f, 1.0f);

	float3x4 transform(mesh_global_transform * offset.Inverted());

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
		float4x4 child_world_transform(mesh_global_transform * it->offset.Inverted());
		child_vec = child_world_transform * child_vec;

		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(child_vec.x, child_vec.y, child_vec.z);
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Sphere
	Sphere joint_sphere(float3(pos.x, pos.y, pos.z), JOINT_SPHERE_RADIUS);
	joint_sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);

	for (std::vector<Joint>::const_iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->DrawBindPos(mesh_global_transform);
}

void Skeleton::Rigg::Joint::SetWorldPositions(const float3x4 & parent_transform)
{
	float3x4 world_transform(parent_transform * current_transform);
	position = world_transform.TranslatePart();

	float3x3 rotation_matrix = world_transform.RotatePart();
	
	rotation_matrix.Orthonormalize(0, 1, 2);

	angles = rotation_matrix.ToEulerXYZ();
	angles *= RADTODEG;

	scaling = world_transform.GetScale();

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->SetWorldPositions(world_transform);
}

void Skeleton::Rigg::Joint::ChangeTransforms(Animation* anim, double anim_time, bool interpolation)
{
	anim->GetJointPos(name, current_transform, offset, anim_time, interpolation);

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->ChangeTransforms(anim, anim_time, interpolation);
}

void Skeleton::Rigg::Joint::GetVertices(const Mesh* original, GLfloat * vertices, const float3x4& parent_mesh)
{
	float3x4 mesh_transform(parent_mesh * current_transform);

	for (std::vector<Weight>::iterator it = weights.begin(); it != weights.end(); ++it)
	{
		float3x4 skining_transform(mesh_transform * offset);

		float3 vertex = skining_transform.TransformPos(original->GetVertex(it->vertex_id));

		vertices[it->vertex_id * 3] += vertex.x * it->influence;
		vertices[it->vertex_id * 3 + 1] += vertex.y * it->influence;
		vertices[it->vertex_id * 3 + 2] += vertex.z * it->influence;
	}

	for (std::vector<Joint>::iterator it = child_joints.begin(); it != child_joints.end(); ++it)
		it->GetVertices(original, vertices, mesh_transform);
}
