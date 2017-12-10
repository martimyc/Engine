#ifndef SKELETON
#define SKELETON

#include "MathGeoLib\src\Math\float3x4.h"
#include "Globals.h"
#include "Resource.h"

class Skeleton : public Resource
{
private:

	struct Rigg
	{
		struct Joint
		{
			float3x4 inverse_bind_pose_transform;
			std::string name;
			uint parent_index;
		};

		std::vector<Joint> joints;
	};

	Rigg* skeleton;

	friend class RiggImporter;

public:
	Skeleton(const std::string& name, const UID& uid);
	Skeleton(const std::string& name, Rigg* source);
	~Skeleton();

private:

public:

};

#endif // !SKELETON

