#ifndef ANIMATION
#define ANIMATION

#include <string>
#include <map>
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
		struct Channel
		{
			std::string joint_name;
			std::vector<std::pair<double,float3>> position_samples;
			std::vector<std::pair<double,Quat>> rotation_samples;
			std::vector<std::pair<double,float3>> scale_samples;

			void GetTransform(double time, float3x4& transform, const float3x4 & inverse_bind_pos, bool interpolation) const;
		};

		double ticks_per_sec;
		double duration;
		std::vector<Channel*> channels;
		bool loop;

		AnimationClip();
		~AnimationClip();

		bool Inspector();

		void GetJointPos(const std::string& joint_name, float3x4& joint_transform, const float3x4 & inverse_bind_pos, double anim_time, bool interpolation = true);
	};

	AnimationClip* source;

	friend class AnimationImporter;
	
public:
	Animation(const std::string& name, const UID& uid);
	Animation(const std::string& name, AnimationClip* source);
	~Animation();

private:

public:
	void SetAnimationClip(AnimationClip* new_clip);

	bool Inspector();

	bool IsLoaded() const;

	void GetJointPos(const std::string& joint_name, float3x4& joint_transform, const float3x4 & inverse_bind_pos, double anim_time, bool interpolation = true);

	double GetLength() const;

	void UnLoad();

	bool GetLoop() const;

	double GetTicksPerSecond() const;
};

#endif // !ANIMATION

