#include "UID.h"
#include "Skeleton.h"
#include "FileSystem.h"
#include "Application.h"
#include "SkeletonImporter.h"

SkeletonImporter::SkeletonImporter()
{}

SkeletonImporter::~SkeletonImporter()
{}

unsigned int SkeletonImporter::GetTotalSize(aiBone** bones, unsigned int num_bones) const
{
	unsigned int size = FORMAT_SIZE + sizeof(unsigned int);

	for(int i = 0; i < num_bones; i++)
	{ 
		size += bones[i]->mName.length + 1;
		size += bones[i]->mNumWeights * (sizeof(unsigned int) + sizeof(float));
	}

	size += (sizeof(float) * 4 * 4 + sizeof(unsigned int)) * num_bones;
	size += sizeof(unsigned int) * num_bones;

	size += sizeof(float) * 4 * 4;

	return size;
}

const aiNode * SkeletonImporter::FindEsqueletonNode(const aiNode* node, aiBone ** bones, unsigned int num_bones) const
{
	if (IsBone(node->mName, bones, num_bones))
		return node;

	const aiNode* ret = nullptr;
	for (int i = 0; i < node->mNumChildren; i++)
	{
		ret = FindEsqueletonNode(node->mChildren[i], bones, num_bones);

		if (ret != nullptr)
			break;
	}
	return ret;
}

const aiBone * SkeletonImporter::FindEsqueletonBone(const aiString & name, aiBone ** bones, unsigned int num_bones) const
{
	for (int i = 0; i < num_bones; i++)
		if (name == bones[i]->mName)
			return bones[i];
	return nullptr;
}

bool SkeletonImporter::IsBone(const aiString & name, aiBone ** bones, unsigned int num_bones) const
{
	for (int i = 0; i < num_bones; i++)
		if (name == bones[i]->mName)
			return true;
	return false;
}

void SkeletonImporter::ImportJoint(char** iterator, const aiBone * bone, const aiNode* node, aiBone** bones, unsigned int num_bones) const
{
	memcpy(*iterator, bone->mName.C_Str(), bone->mName.length + 1);
	*iterator += bone->mName.length + 1;

	memcpy(*iterator, &bone->mOffsetMatrix.a1, sizeof(float) * 4 * 4);
	*iterator += sizeof(float) * 4 * 4;

	memcpy(*iterator, &bone->mNumWeights, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < bone->mNumWeights; i++)
	{
		memcpy(*iterator, &bone->mWeights[i].mVertexId, sizeof(unsigned int) + sizeof(float));
		*iterator += sizeof(unsigned int) + sizeof(float);
	}

	//Check all are bones
	unsigned int num_bone_childs = 0;
	for (int i = 0; i < node->mNumChildren; i++)
		if (IsBone(node->mChildren[i]->mName, bones, num_bones))
			num_bone_childs++;

	memcpy(*iterator, &num_bone_childs, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < node->mNumChildren; i++)
		if (IsBone(node->mChildren[i]->mName, bones, num_bones))
		{
			const aiBone* child_bone = FindEsqueletonBone(node->mChildren[i]->mName, bones, num_bones);
			ImportJoint(iterator, child_bone, node->mChildren[i], bones, num_bones);
		}
}

Skeleton::Rigg::Joint SkeletonImporter::LoadJoint(char ** iterator) const
{
	Skeleton::Rigg::Joint joint;

	joint.name = *iterator;
	*iterator += joint.name.length() + 1;

	for (int i = 0; i < 4; i++)
	{
		memcpy(&joint.inverse_bind_pose_transform[i][0], *iterator, sizeof(float) * 4);
		*iterator += sizeof(float) * 4;
	}
	
	if (joint.inverse_bind_pose_transform.Inverse() == false)
		LOG("Could not inverse joint transform");

	unsigned int num_weights;
	memcpy(&num_weights, *iterator, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	joint.weights.resize(num_weights);
	memcpy(&joint.weights[0], *iterator, (sizeof(unsigned int) + sizeof(float)) * num_weights);
	*iterator += (sizeof(unsigned int) + sizeof(float)) * num_weights;

	unsigned int num_childs = **iterator;
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < num_childs; i++)
		joint.child_joints.push_back(LoadJoint(iterator));

	return joint;
}

const UID SkeletonImporter::Import(aiBone ** bones, aiNode* scene_root_node, unsigned int num_bones) const
{
	char format[FORMAT_SIZE] = FORMAT_SKELETON;
	unsigned int buff_size = GetTotalSize(bones, num_bones);
	char* buffer = new char[buff_size];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	const aiNode* root_node = FindEsqueletonNode(scene_root_node, bones, num_bones);
	const aiBone* root_joint = FindEsqueletonBone(root_node->mName, bones, num_bones);

	aiMatrix4x4 transform = root_node->mTransformation;
	const aiNode* node = root_node;
	while (node->mParent != nullptr)
	{
		transform = transform * node->mParent->mTransformation;
		node = node->mParent;
	}

	memcpy(iterator, &transform.a1, sizeof(float) * 4 * 4);
	iterator += sizeof(float) * 4 * 4;

	ImportJoint(&iterator, root_joint, root_node, bones, num_bones);

	//take skeleton out of hirarchy
	aiNode* parent = root_node->mParent;

	for (int i = 0; i < parent->mNumChildren; i++)
		if (parent->mChildren[i] == root_node)
		{
			parent->mChildren[i] = nullptr;

			//This will remove non bone nodes if there are any within the rigg
			for (int j = i + 1; j < parent->mNumChildren; j++)
				parent->mChildren[j - 1] = parent->mChildren[j];

			parent->mNumChildren--;
			break;
		}

	uint length = iterator - buffer;

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, App->file_system->GetSkeletons().c_str(), id.GetAsName(), "mm") == false)
	{
		LOG("Could not save skeleton correctlly");
		return UID();
	}

	delete[] buffer;

	return id;
}

bool SkeletonImporter::Load(Skeleton * to_load, const SkeletonLoadConfiguration* config) const
{
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetSkeletons());
	path += "\\";
	path += to_load->GetUID().GetAsName();
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		Skeleton::Rigg* new_rigg = new Skeleton::Rigg;

		for (int i = 0; i < 4; i++)
		{
			memcpy(&new_rigg->skeleton_transform[i][0], iterator, sizeof(float) * 4);
			iterator += sizeof(float) * 4;
		}

		new_rigg->root_joint = LoadJoint(&iterator);

		to_load->SetRigg(new_rigg);

		return true;
	}

	LOG("Could not read %s", path.c_str());
	return false;
}
