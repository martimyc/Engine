#ifndef SKELETON_IMPORTER
#define SKELETON_IMPORTER

#include <string>
#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "glew\include\GL\glew.h"

class UID;
class AssetDirectory;
class Skeleton;

struct SkeletonImportConfiguration;
struct SkeletonLoadConfiguration;

class SkeletonImporter
{
public:
	SkeletonImporter();
	~SkeletonImporter();

private:
	unsigned int GetTotalSize(aiBone** bones, unsigned int num_bones) const;

	void ImportJoint(char** iterator, const aiBone* bone) const;

public:
	const UID Import(aiBone** bones, unsigned int num_bones) const;
	bool Load(Skeleton* to_load, const SkeletonLoadConfiguration* config) const;
};

#endif
