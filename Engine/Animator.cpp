#include <algorithm>
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
	skeleton->DeformableMesh(game_object->GetMesh());
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

		for (std::vector<std::pair<float,Animation*>>::iterator it = animations.begin(); it != animations.end(); ++it)
		{
			it->second->Inspector();

			if (ImGui::DragFloat("Start Time", &it->first))
				std::sort(animations.begin(), animations.end(), CompareAnims());
		}

		ImGui::TreePop();
	}
}

void Animator::LoadComponent(char ** iterator, const GameObject* game_object)
{
	//Animations	
	unsigned int num_animations = 0;
	memcpy(&num_animations, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_animations; ++i)
	{
		double d = 0.0f;
		memcpy(&d, *iterator, sizeof(double));
		*iterator += sizeof(double);

		//Animation Name
		unsigned int name_lenght = 0;
		memcpy(&name_lenght, *iterator, sizeof(unsigned int));
		*iterator += sizeof(unsigned int);

		char name[128];
		memcpy(name, *iterator, name_lenght);
		*iterator += name_lenght;

		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		// = new Animation(name, uid);
		 Animation* animation = App->resource_manager->UseAnimation(uid, game_object);
		animations.insert(std::make_pair(d, animation));
	}

	//Bools
	memcpy(&draw_skeleton, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&draw_bind_pos, *iterator, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(&interpolation, *iterator, sizeof(bool));
	*iterator += sizeof(bool);
}

void Animator::SaveComponent(char ** iterator) const
{
	COMPONENT_TYPE component_type = CT_ANIMATOR;
	memcpy(*iterator, &component_type, sizeof(COMPONENT_TYPE));
	*iterator += sizeof(COMPONENT_TYPE);

	memcpy(*iterator, &skeleton->GetUID(), SIZE_OF_UID);
	*iterator += SIZE_OF_UID;

	//Animations	
	unsigned int num_animations = animations.size();
	memcpy(*iterator, &num_animations, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (std::multimap<double, Animation*>::const_iterator it = animations.begin(); it != animations.end(); ++it)
	{
		memcpy(*iterator, &it->first, sizeof(double));
		*iterator += sizeof(double);

		//Animation Name
		std::string name(it->second->GetName().c_str());
		unsigned int name_lenght = name.length() + 1;

		memcpy(*iterator, &name_lenght, sizeof(unsigned int));
		*iterator += sizeof(unsigned int);

		memcpy(*iterator, name.c_str(), name_lenght);
		*iterator += name_lenght;

		memcpy(*iterator, &it->second->GetUID(), SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
	}

	//Bools
	memcpy(*iterator, &draw_skeleton, sizeof(bool));
	*iterator += sizeof(bool);
	
	memcpy(*iterator, &draw_bind_pos, sizeof(bool));
	*iterator += sizeof(bool);

	memcpy(*iterator, &interpolation, sizeof(bool));
	*iterator += sizeof(bool);
}

void Animator::AddAnimation(Animation * new_anim, float start_time)
{
	animations.push_back(std::make_pair(start_time,new_anim));
	std::sort(animations.begin(), animations.end(), CompareAnims());
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
			double anim_time = (int)(time / animations.begin()->second->GetLength());
			anim_time = time - anim_time * animations.begin()->second->GetLength();
			double tick = anim_time * animations.begin()->second->GetTicksPerSecond();
			skeleton->ChangeJointTransforms(animations.begin()->second, tick, interpolation);

			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}

			skeleton->UpdateMesh(game_object->GetMesh());
		}
		else if (time >= begin && time <= end)
		{
			double tick = (time - animations.begin()->first) * animations.begin()->second->GetTicksPerSecond();
			skeleton->ChangeJointTransforms(animations.begin()->second, tick, interpolation);

			if (draw_skeleton)
			{
				float4x4 world_transform(game_object->GetWorldTransform().Transposed());
				float3x4 mesh_world_transform(world_transform.Col3(0), world_transform.Col3(1), world_transform.Col3(2), world_transform.Col3(3));
				skeleton->UpdateJointTransforms(mesh_world_transform);
			}

			skeleton->UpdateMesh(game_object->GetMesh());
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
	for (std::vector<std::pair<float, Animation*>>::iterator it = animations.begin(); it != animations.end(); ++it)
		App->resource_manager->StopUsingAnimation(it->second, go);
}
