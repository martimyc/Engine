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
	std::vector<std::pair<float,Animation*>> animations;

	//debug
	bool draw_skeleton;
	bool draw_bind_pos;
	bool interpolation;
	bool in_place;
	bool blend;

	//presentation
	float weight = 0.0f;
	bool weight_up = false;
	bool weight_down = false;

public:
	Animator(Skeleton* skeleton, const GameObject* const go, bool enabled = true);
	Animator(const Animator& copy, const GameObject* const go);
	~Animator();

	const std::string& GetName() const;

	void Inspector();

	void LoadComponent(char** iterator, const GameObject* game_object = nullptr);
	void SaveComponent(char** iterator)const;

	void AddAnimation(Animation* new_anim, float start_time);

	void SetSkeleton(Skeleton* new_skeleton);

	const Skeleton* GetSkeleton() const;

	void DrawSkeleton() const;

	bool Update();

	void ChangeSkeleton(Skeleton* new_skeleton);

	void StopUsingAnimation(const GameObject* go);
};

struct CompareAnims
{
	bool operator() (std::pair<double, Animation*> i, std::pair<double, Animation*> j) { return (i.first < j.first); }
};


#endif //!ANIMATOR
