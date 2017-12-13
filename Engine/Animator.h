#ifndef ANIMATOR
#define ANIMATOR

#include <vector>
#include <utility>
#include "Component.h"

class Skeleton;
class Animation;

class Animator : public Component
{
private:
	Skeleton* skeleton;
	std::vector<std::pair<Animation*, double>> animations;

	//debug
	bool draw_skeleton;

public:
	Animator(Skeleton* skeleton, const GameObject* const go, bool enabled = true);
	Animator(const Animator& copy, const GameObject* const go);
	~Animator();

	const std::string& GetName() const;

	void Inspector();

	void AddAnimation(Animation* new_anim, double start_time);

	void SetSkeleton(Skeleton* new_skeleton);

	void GetAnimations( std::vector<const Animation*>& vec_to_fill) const;

	const Skeleton* GetSkeleton() const;

	void DrawSkeleton(const GLfloat* opengl_view_matrix) const;
};


#endif //!ANIMATOR
