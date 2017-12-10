#include "Skeleton.h"

Skeleton::Skeleton(const std::string & name, const UID & uid) : Resource(RT_SKELETON, name, uid)
{}

Skeleton::Skeleton(const std::string & name, Rigg * source) : Resource(RT_SKELETON, name), skeleton(source)
{}

Skeleton::~Skeleton()
{
	delete skeleton;
}
