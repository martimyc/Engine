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
	aiNode* FindEsqueletonNode(aiNode* node, aiBone** bones, unsigned int num_bones) const;
	aiBone* FindEsqueletonBone(const aiString& name, aiBone** bones, unsigned int num_bones) const;
	bool IsBone(const aiString& name, aiBone ** bones, unsigned int num_bones) const;
	void ColapseUselesNodes(aiNode* skeleton_root) const;
	aiMatrix4x4 GetGlobalTransform(const aiNode* node) const;

	void ImportJoint(char** iterator, const aiBone* bone, const aiNode* node, aiBone** bones, unsigned int num_bones) const;
	Skeleton::Rigg::Joint LoadJoint(char** iterator, const float3x4& parent_transform) const;

public:
	const UID Import(aiBone** bones, aiNode* scene_root_node, aiNode* mesh_node, unsigned int num_bones) const;
	bool Load(Skeleton* to_load, const SkeletonLoadConfiguration* config) const;
};

#endif
