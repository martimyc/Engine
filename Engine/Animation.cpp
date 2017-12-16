#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Animation.h"

Animation::AnimationClip::AnimationClip()
{}

Animation::AnimationClip::~AnimationClip()
{
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete *it;
	channels.clear();
}

bool Animation::AnimationClip::Inspector()
{
	ImGui::Text("FPS: %f\nFrame Count:%i\nNum Channels: %i", fps, frame_count, channels.size());
	ImGui::Checkbox("Loop",&loop);

	return false;
}

void Animation::AnimationClip::GetSkeletonPositions(double time, std::vector<std::pair<std::string, float3x4>>& transforms, bool interpolation) const
{
	float total_seconds = frame_count / fps;

	if (loop)
		time -= ((int)time / total_seconds) * total_seconds;

	for (int i = 0; i > channels.size(); i++)
	{
		transforms.push_back(std::make_pair(channels[i]->joint_name, float3x4::identity));
		channels[i]->GetTransform( time, transforms[i].second, interpolation);
	}
}

double Animation::AnimationClip::GetLength() const
{
	return frame_count / fps;
}

Animation::Animation(const std::string & name, const UID & uid): Resource(RT_ANIMATION, name, uid), source(nullptr)
{}

Animation::Animation(const std::string & name, AnimationClip * source): Resource(RT_ANIMATION, name), source(source)
{}

Animation::~Animation()
{
	if (source != nullptr)
		delete source;
}

void Animation::SetAnimationClip(AnimationClip * new_clip)
{
	source = new_clip;
}

bool Animation::Inspector()
{
	if (source != nullptr)
	{
		bool ret = false;
		if (ImGui::TreeNodeEx(name.c_str()))
		{
			ret = source->Inspector();
			ImGui::TreePop();
		}
		return ret;
	}
	LOG("Source not loaded");
	return false;
}

bool Animation::IsLoaded() const
{
	return source != nullptr;
}

void Animation::GetSkeletonPositions(double time, std::vector<std::pair<std::string, float3x4>>& transforms, bool interpolation)const
{
	if (source != nullptr)
		source->GetSkeletonPositions(time, transforms, interpolation);
	else
		LOG("Source not loaded");
}

double Animation::GetLength() const
{
	if (source != nullptr)
		return source->GetLength();
	else
		LOG("Source not loaded");
}

void Animation::AnimationClip::Channel::GetTransform(double time, float3x4 & transform, bool interpolation) const
{
	transform.SetIdentity();
	
	//Position
	std::vector<std::pair<double, float3>>::const_iterator upper_bound;
	std::vector<std::pair<double, float3>>::const_iterator lower_bound;

	if(position_samples.size() != 0)
	{
		for (lower_bound = position_samples.begin(); lower_bound != position_samples.end(); ++lower_bound)
			if ((lower_bound + 1)->first > time && lower_bound->first < time)
			{
				upper_bound = lower_bound + 1;
				break;
			}

		if (interpolation == false)
			transform.SetTranslatePart(lower_bound->second);
		else
		{
			float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
			transform.SetTranslatePart(lower_bound->second.Lerp(upper_bound->second, weight));
		}
	}

	//Rotation
	if (rotation_samples.size() != 0)
	{
		std::vector<std::pair<double, Quat>>::const_iterator upper_bound_rot;
		std::vector<std::pair<double, Quat>>::const_iterator lower_bound_rot;

		for (lower_bound_rot = rotation_samples.begin(); lower_bound_rot != rotation_samples.end(); ++lower_bound_rot)
			if ((lower_bound_rot + 1)->first > time && lower_bound_rot->first < time)
			{
				upper_bound_rot = lower_bound_rot + 1;
				break;
			}

		if (interpolation == false)
			transform.SetRotatePart(lower_bound_rot->second);
		else
		{
			float weight = (time - lower_bound_rot->first) / (upper_bound_rot->first - lower_bound_rot->first);
			transform.SetRotatePart(lower_bound_rot->second.Slerp(upper_bound_rot->second, weight));
		}
	}

	//Scaling
	if (scale_samples.size() != 0)
	{
		for (lower_bound = scale_samples.begin(); lower_bound != scale_samples.end(); ++lower_bound)
			if ((lower_bound + 1)->first > time && lower_bound->first < time)
			{
				upper_bound = lower_bound + 1;
				break;
			}

		if (interpolation == false)
			transform.Scale(lower_bound->second, transform.TranslatePart());
		else
		{
			float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
			transform.Scale(lower_bound->second.Lerp(upper_bound->second, weight), transform.TranslatePart());
		}
	}
}

void Animation::UnLoad()
{
	delete source;
	source = nullptr;
}
