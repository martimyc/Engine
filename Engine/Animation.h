#ifndef ANIMATION
#define ANIMATION

#include <string>
#include <vector>
#include "MathGeoLib\src\Math\float3.h"
#include "MathGeoLib\src\Math\Quat.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Resource.h"


class Animation : public Resource
{
private:
	struct AnimationClip
	{	
		struct JointPose
		{
			float3 position;
			Quat rotation;
			float scale;
		};

		struct AnimationSample
		{
			std::vector<JointPose> joint_poses; //maybe useless
			std::vector<float4x4> global_joint_poses;

			void Draw(const GLfloat* opengl_view_matrix) const;
		};

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

