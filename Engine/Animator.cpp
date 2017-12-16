#include "MathGeoLib\src\Math\float3x3.h"
#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "imgui\imgui.h"
#include "GameObject.h"
#include "Animation.h"
#include "Skeleton.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "Application.h"
#include "Animator.h"

Animator::Animator(Skeleton * skeleton, const GameObject* const go, bool enabled): Component(CT_ANIMATOR, go, enabled), skeleton(skeleton), draw_skeleton(false), draw_bind_pos(false)
{
	float4x4 world_transform(game_object->GetWorldTransform().Transposed());
	float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
	skeleton->SetWorldPositions(mesh_world_transform);
}

Animator::Animator(const Animator & copy, const GameObject* const go): Component(CT_ANIMATOR, go, copy.enabled), draw_skeleton(copy.draw_skeleton), draw_bind_pos(copy.draw_bind_pos)
{
	skeleton = App->resource_manager->UseSkeleton(copy.skeleton->GetUID(), game_object);

	float4x4 world_transform(game_object->GetWorldTransform().Transposed());
	float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
	skeleton->SetWorldPositions(mesh_world_transform); //No time, this makes more than one of a skeleton instance imposible to handle
}

Animator::~Animator()
{}

const std::string & Animator::GetName() const
{
	return game_object->GetName() + "_Animator";
}

void Animator::Inspector()
{
	if (ImGui::TreeNode("Animator"))
	{
		ImGui::Checkbox("Draw Skeleton", &draw_skeleton);
		ImGui::Checkbox("Draw Bind Position", &draw_bind_pos);

		if (skeleton->Inspector())
		{
			float4x4 world_transform(game_object->GetWorldTransform().Transposed());
			float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
			skeleton->UpdateJointTransforms(mesh_world_transform);
		}

		ImGui::TreePop();
	}
}

void Animator::AddAnimation(Animation * new_anim, double start_time)
{
	animations.push_back(std::make_pair(new_anim, start_time));
}

void Animator::SetSkeleton(Skeleton * new_skeleton)
{
	skeleton = new_skeleton;
}

void Animator::GetAnimations(std::vector<const Animation*>& vec_to_fill) const
{
	for (std::vector<std::pair<Animation*, double>>::const_iterator it = animations.begin(); it != animations.end(); ++it)
		vec_to_fill.push_back(it->first);
}

const Skeleton * Animator::GetSkeleton() const
{
	return skeleton;
}

void Animator::DrawSkeleton() const
{
	if (draw_skeleton)
	{
		float4x4 world_transform(game_object->GetWorldTransform().Transposed());
		float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
		skeleton->Draw(mesh_world_transform);
	}

	if (draw_bind_pos)
	{
		float4x4 world_transform(game_object->GetWorldTransform().Transposed());
		float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
		skeleton->DrawBindPos(mesh_world_transform);
	}
}

bool Animator::Update()
{
	std::vector<std::pair<std::string, float3x4>> joint_transforms;

	float time = App->time_manager->GetGameTimeSinceStart();

	//TODO animation blending
	if (animations.size() != 0)
	{
		if (animations[0].second > time && animations[0].second < time + animations[0].first->GetLength())
		{
			animations[0].first->GetSkeletonPositions(time, joint_transforms);
			skeleton->ChangeJointTransforms(joint_transforms);
		}
	}
	return true;
}

void Animator::ChangeSkeleton(Skeleton* new_skeleton)
{
	App->resource_manager->StopUsingSkeleton(skeleton, game_object);
	skeleton = new_skeleton;
}

void Animator::StopUsingAnimation(const GameObject * go)
{
	App->resource_manager->StopUsingSkeleton(skeleton, go);
	//App->resource_manager->StopUsingAnimation(animations, go);
}
