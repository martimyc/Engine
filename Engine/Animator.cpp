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

Animator::Animator(Skeleton * skeleton, const GameObject* const go, bool enabled): Component(CT_ANIMATOR, go, enabled), skeleton(skeleton), draw_skeleton(false), draw_bind_pos(false), interpolation(true)
{
	float4x4 world_transform(game_object->GetWorldTransform().Transposed());
	float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
	skeleton->SetWorldPositions(mesh_world_transform);
	skeleton->UpdateJointTransforms(mesh_world_transform);
	skeleton->OriginalMesh(game_object->GetMesh());
}

Animator::Animator(const Animator & copy, const GameObject* const go): Component(CT_ANIMATOR, go, copy.enabled), draw_skeleton(copy.draw_skeleton), draw_bind_pos(copy.draw_bind_pos)
{
	skeleton = App->resource_manager->UseSkeleton(copy.skeleton->GetUID(), game_object);

	float4x4 world_transform(game_object->GetWorldTransform().Transposed());
	float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
	//No time, this makes more than one of a skeleton instance imposible to handle
	skeleton->SetWorldPositions(mesh_world_transform);
	skeleton->UpdateJointTransforms(mesh_world_transform);
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
		if(ImGui::Checkbox("Draw Skeleton", &draw_skeleton))
			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}

		ImGui::Checkbox("Draw Bind Position", &draw_bind_pos);
		ImGui::Checkbox("Interpolation", &interpolation);

		if (skeleton->Inspector())
		{
			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}
		}

		ImGui::Text("Num Animations: %i", animations.size());

		for (std::multimap<double,Animation*>::iterator it = animations.begin(); it != animations.end(); ++it)
		{
			it->second->Inspector();
			float insert(it->first);

			if (ImGui::DragFloat("Start Time", &insert))
			{
				Animation* anim = it->second;
				animations.erase(it);
				animations.insert(std::make_pair(double(insert), anim));
			}
		}

		ImGui::TreePop();
	}
}

void Animator::AddAnimation(Animation * new_anim, float start_time)
{
	animations.insert(std::make_pair(double(start_time),new_anim));
}

void Animator::SetSkeleton(Skeleton * new_skeleton)
{
	skeleton = new_skeleton;
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
	float time = App->time_manager->GetGameTimeSinceStart();

	//TODO animation blending
	if (animations.size() != 0)
	{
		double begin = animations.begin()->first;
		double end = animations.begin()->first + animations.begin()->second->GetLength();

		if (animations.begin()->second->GetLoop() == true)
		{
			double anim_time = time - (int)(time / animations.begin()->second->GetLength());
			anim_time = anim_time - animations.begin()->first;
			skeleton->ChangeJointTransforms(animations.begin()->second, anim_time, interpolation);

			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}
		}
		if (time >= begin && time <= end)
		{
			double tick = (time - animations.begin()->first) * animations.begin()->second->GetTicksPerSecond();
			skeleton->ChangeJointTransforms(animations.begin()->second, tick, interpolation);

			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}
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
