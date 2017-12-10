#ifndef ANIMATION
#define ANIMATION

#include <string>
#include <vector>
#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Resource.h"


class Animation : public Resource
{
private:
	struct AnimationClip
	{
		struct Skeleton
		{
			struct Joint
			{
				float3x4 inverse_bind_pose_transform;
				std::string name;
				uint parent_index;
			};

			std::vector<Joint> joints;
		};

		struct JointPose
		{
			float3 position;
			Quat rotation;
			float scale;
		};

		struct AnimationSample
		{
			std::vector<JointPose> joint_poses;
			std::vector<float4x4> global_joint_poses;

			void Draw(const GLfloat* opengl_view_matrix) const;
		};

		Skeleton* skeleton;
		float fps;
		uint frame_count;
		std::vector<AnimationSample*> samples;
		bool loop;

		AnimationClip();
		~AnimationClip();
	};

	AnimationClip* source;
	friend class AnimationImporter;
	
public:
	Animation(const std::string& name, const UID& uid);
	Animation(const std::string& name, AnimationClip* source);
	~Animation();

private:

public:
	
};

#endif // !ANIMATION

