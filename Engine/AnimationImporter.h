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
	bool Load(Animation* to_load, const AnimationLoadConfiguration* config) const;
};

struct CompareVec
{
	bool operator() (std::pair<double, float3> i, std::pair<double, float3> j) { return (i.first < j.first); }
};

struct CompareQuat
{
	bool operator() (std::pair<double, Quat> i, std::pair<double, Quat> j) { return (i.first < j.first); }
};

#endif //!ANIMATION_IMPORTER