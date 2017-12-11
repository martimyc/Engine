#include "Animation.h"

Animation::AnimationClip::AnimationClip()
{
}

Animation::AnimationClip::~AnimationClip()
{
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
	return false;
}

bool Animation::IsLoaded() const
{
	return source != nullptr;
}
