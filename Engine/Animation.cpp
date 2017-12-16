#include "imgui\imgui.h"
#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Math\float3.h"
#include "Animation.h"

Animation::AnimationClip::AnimationClip()
{}

Animation::AnimationClip::~AnimationClip()
{}

bool Animation::AnimationClip::Inspector()
{
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

Animation::Animation(const std::string & name, const UID & uid): Resource(RT_ANIMATION, name, uid)
{}

Animation::Animation(const std::string & name, AnimationClip * source): Resource(RT_ANIMATION, name)
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
		return source->Inspector();
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
	for(int i = 0; i < position_samples.size(); i++)
		if (position_samples[i]->time > time && position_samples[i + 1]->time < time)
		{
			if (interpolation == false)
				transform.SetTranslatePart(position_samples[i]->position);
			else
			{
				float weight = (time - position_samples[i]->time) / (position_samples[i + 1]->time - position_samples[i]->time);
				transform.SetTranslatePart(position_samples[i]->position.Lerp(position_samples[i + 1]->position, weight));
			}
			break;
		}

	//Rotation
	for (int i = 0; i < rotation_samples.size(); i++)
		if (rotation_samples[i]->time > time && rotation_samples[i + 1]->time < time)
		{
			if (interpolation == false)
				transform.SetRotatePart(rotation_samples[i]->rotation);
			else
			{
				float weight = (time - rotation_samples[i]->time) / (rotation_samples[i + 1]->time - rotation_samples[i]->time);
				transform.SetRotatePart(rotation_samples[i]->rotation.Slerp(rotation_samples[i + 1]->rotation, weight));
			}
			break;
		}

	//Scaling
	for (int i = 0; i < scale_samples.size(); i++)
		if (scale_samples[i]->time > time && scale_samples[i + 1]->time < time)
		{
			if (interpolation == false)
				transform.SetTranslatePart(scale_samples[i]->scale);
			else
			{
				float weight = (time - scale_samples[i]->time) / (scale_samples[i + 1]->time - scale_samples[i]->time);
				transform.Scale(scale_samples[i]->scale.Lerp(scale_samples[i + 1]->scale, weight), transform.TranslatePart());
			}
			break;
		}
}

void Animation::UnLoad()
{
	delete source;
	source = nullptr;
}
