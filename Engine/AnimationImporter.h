#ifndef ANIMATION_IMPORTER
#define ANIMATION_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"

class UID;
class Animation;
class AssetDirectory;
struct AnimationImportConfiguration;
struct AnimationLoadConfiguration;

class AnimationImporter
{
public:
	AnimationImporter();
	~AnimationImporter();

private:
	unsigned int GetTotalSize(const aiAnimation * animation) const;

public:
	const UID Import(const aiAnimation* animation, const AnimationImportConfiguration* config) const;
	bool Load(Animation* to_load, unsigned int priority, const AnimationLoadConfiguration* config) const;
};

#endif //!ANIMATION_IMPORTER