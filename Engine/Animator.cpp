#include "imgui\imgui.h"
#include "GameObject.h"
#include "Animation.h"
#include "Skeleton.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Animator.h"

Animator::Animator(Skeleton * skeleton, const GameObject* const go, bool enabled): Component(CT_ANIMATOR, go, enabled), skeleton(skeleton), draw_skeleton(false), draw_bind_pos(false)
{}

Animator::Animator(const Animator & copy, const GameObject* const go): Component(CT_ANIMATOR, go, copy.enabled), draw_skeleton(copy.draw_skeleton), draw_bind_pos(copy.draw_bind_pos)
{
	skeleton = App->resource_manager->UseSkeleton(copy.skeleton->GetUID(), game_object);
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
	if(draw_skeleton)
		skeleton->Draw(game_object->GetWorldTransform());
	if (draw_bind_pos)
		skeleton->DrawBindPos(game_object->GetWorldTransform());
}
