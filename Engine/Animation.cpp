#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Animation.h"

Animation::AnimationClip::AnimationClip(): loop(false)
{}

Animation::AnimationClip::~AnimationClip()
{
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete *it;
	channels.clear();
}

bool Animation::AnimationClip::Inspector()
{
	float duration_secs = duration / ticks_per_sec;
	ImGui::Text("Ticks per second: %f\nDuration: %f sec\nNum Channels: %i", ticks_per_sec, duration_secs, channels.size());
	ImGui::Checkbox("Loop",&loop);

	return false;
}

void Animation::AnimationClip::GetJointPos(const std::string & joint_name, float3x4 & joint_transform, const float3x4 & offset, double anim_time, bool interpolation) const
{
	for (int i = 0; i < channels.size(); i++)
		if (channels[i]->joint_name == joint_name)
			channels[i]->GetTransform(anim_time, joint_transform, offset, interpolation);
}

void Animation::AnimationClip::GetTRS(float3 & pos, Quat & rot, float3 & scale, const std::string & joint_name, const float3x4 & offset, double anim_time, bool interpolation) const
{
	for (int i = 0; i < channels.size(); i++)
		if (channels[i]->joint_name == joint_name)
			channels[i]->GetTRS(pos, rot, scale, anim_time, offset, interpolation);
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

void Animation::GetJointPos(const std::string & joint_name, float3x4 & joint_transform, const float3x4 & offset, double anim_time, bool interpolation) const
{
	if (source != nullptr)
		source->GetJointPos(joint_name, joint_transform, offset, anim_time, interpolation);
	else
		LOG("Source not loaded");
}

void Animation::GetTRS(float3 & pos, Quat & rot, float3 & scale, const std::string & joint_name, const float3x4 & offset, double anim_time, bool interpolation) const
{
	if (source != nullptr)
		source->GetTRS(pos, rot, scale, joint_name, offset, anim_time, interpolation);
	else
		LOG("Source not loaded");
}

double Animation::GetLength() const
{
	if (source != nullptr)
		return source->duration / source->ticks_per_sec;
	else
		LOG("Source not loaded");
}

void Animation::AnimationClip::Channel::GetTransform(double time, float3x4 & transform, const float3x4 & inverse_bind_pos, bool interpolation) const
{
	float3 position(float3::zero);
	Quat rotation(Quat::identity);
	float3 scale(float3::one);

	//Position
	std::vector<std::pair<double, float3>>::const_iterator upper_bound;
	std::vector<std::pair<double, float3>>::const_iterator lower_bound;

	if(position_samples.size() != 0)
	{
		if (position_samples.size() == 1)
			position = position_samples.begin()->second;
		else
		{
			for (lower_bound = position_samples.begin(); lower_bound != position_samples.end(); ++lower_bound)
			{
				upper_bound = lower_bound + 1;
				if (upper_bound->first >= time && lower_bound->first <= time)
					break;
			}

			if (interpolation == false)
				position = lower_bound->second;
			else
			{
				float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
				position = lower_bound->second.Lerp(upper_bound->second, weight);
			}
		}
	}

	//Rotation
	if (rotation_samples.size() != 0)
	{
		if (rotation_samples.size() == 1)
			rotation = rotation_samples.begin()->second;
		else
		{
			std::vector<std::pair<double, Quat>>::const_iterator upper_bound_rot;
			std::vector<std::pair<double, Quat>>::const_iterator lower_bound_rot;

			for (lower_bound_rot = rotation_samples.begin(); lower_bound_rot != rotation_samples.end(); ++lower_bound_rot)
			{
				upper_bound_rot = lower_bound_rot + 1;
				if (upper_bound_rot->first >= time && lower_bound_rot->first <= time)
					break;
			}

			if (interpolation == false)
				rotation = lower_bound_rot->second;
			else
			{
				float weight = (time - lower_bound_rot->first) / (upper_bound_rot->first - lower_bound_rot->first);
				rotation = lower_bound_rot->second.Slerp(upper_bound_rot->second, weight);
				rotation.Normalize();
			}
		}
	}

	//Scaling
	if (scale_samples.size() != 0)
	{
		if (scale_samples.size() == 1)
			scale = scale_samples.begin()->second, transform.TranslatePart();
		else
		{
			for (lower_bound = scale_samples.begin(); lower_bound != scale_samples.end(); ++lower_bound)
			{
				upper_bound = lower_bound + 1;
				if ((lower_bound + 1)->first >= time && lower_bound->first <= time)
					break;
			}

			if (interpolation == false)
				scale = lower_bound->second, transform.TranslatePart();
			else
			{
				float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
				scale = lower_bound->second.Lerp(upper_bound->second, weight), transform.TranslatePart();
			}
		}
	}

	transform = float3x4::FromTRS(position, rotation, scale);
}

void Animation::AnimationClip::Channel::GetTRS(float3 & position, Quat & rotation, float3 & scale, double time, const float3x4 & offset, bool interpolation) const
{
	//Position
	std::vector<std::pair<double, float3>>::const_iterator upper_bound;
	std::vector<std::pair<double, float3>>::const_iterator lower_bound;

	if (position_samples.size() != 0)
	{
		if (position_samples.size() == 1)
			position = position_samples.begin()->second;
		else
		{
			for (lower_bound = position_samples.begin(); lower_bound != position_samples.end(); ++lower_bound)
			{
				upper_bound = lower_bound + 1;
				if (upper_bound->first >= time && lower_bound->first <= time)
					break;
			}

			if (interpolation == false)
				position = lower_bound->second;
			else
			{
				float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
				position = lower_bound->second.Lerp(upper_bound->second, weight);
			}
		}
	}

	//Rotation
	if (rotation_samples.size() != 0)
	{
		if (rotation_samples.size() == 1)
			rotation = rotation_samples.begin()->second;
		else
		{
			std::vector<std::pair<double, Quat>>::const_iterator upper_bound_rot;
			std::vector<std::pair<double, Quat>>::const_iterator lower_bound_rot;

			for (lower_bound_rot = rotation_samples.begin(); lower_bound_rot != rotation_samples.end(); ++lower_bound_rot)
			{
				upper_bound_rot = lower_bound_rot + 1;
				if (upper_bound_rot->first >= time && lower_bound_rot->first <= time)
					break;
			}

			if (interpolation == false)
				rotation = lower_bound_rot->second;
			else
			{
				float weight = (time - lower_bound_rot->first) / (upper_bound_rot->first - lower_bound_rot->first);
				rotation = lower_bound_rot->second.Slerp(upper_bound_rot->second, weight);
				rotation.Normalize();
			}
		}
	}

	//Scaling
	if (scale_samples.size() != 0)
	{
		if (scale_samples.size() == 1)
			scale = scale_samples.begin()->second, position;
		else
		{
			for (lower_bound = scale_samples.begin(); lower_bound != scale_samples.end(); ++lower_bound)
			{
				upper_bound = lower_bound + 1;
				if ((lower_bound + 1)->first >= time && lower_bound->first <= time)
					break;
			}

			if (interpolation == false)
				scale = lower_bound->second, position;
			else
			{
				float weight = (time - lower_bound->first) / (upper_bound->first - lower_bound->first);
				scale = lower_bound->second.Lerp(upper_bound->second, weight), position;
			}
		}
	}
}

void Animation::UnLoad()
{
	delete source;
	source = nullptr;
}

bool Animation::GetLoop() const
{
	if (source != nullptr)
		return source->loop;
	LOG("Source not loaded");
	return false;
}

double Animation::GetTicksPerSecond() const
{
	if (source != nullptr)
		return source->ticks_per_sec;
	LOG("Source not loaded");
	return 0.0;
}
