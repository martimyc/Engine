#ifndef ANIMATION_ASSET
#define ANIMATION_ASSET

#include "Asset.h"

class AnimationAsset : public Asset
{
private:
	std::vector<const GameObject*> instances;
	GLuint image;

public:

};


#endif // !ANIMATION_ASSET

