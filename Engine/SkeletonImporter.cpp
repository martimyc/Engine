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

	size += (2 * sizeof(float) * 4 * 4 + sizeof(unsigned int)) * num_bones ;
	size += sizeof(unsigned int) * num_bones;

	return size;
}

aiNode * SkeletonImporter::FindEsqueletonNode(aiNode* node, aiBone ** bones, unsigned int num_bones) const
{
	if (IsBone(node->mName, bones, num_bones))
		return node;

	aiNode* ret = nullptr;
	for (int i = 0; i < node->mNumChildren; i++)
	{
		ret = FindEsqueletonNode(node->mChildren[i], bones, num_bones);

		if (ret != nullptr)
			break;
	}
	return ret;
}

aiBone * SkeletonImporter::FindEsqueletonBone(const aiString & name, aiBone ** bones, unsigned int num_bones) const
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

void SkeletonImporter::ColapseUselesNodes(aiNode * skeleton_root) const
{
	if (skeleton_root->mParent->mNumChildren == 1 && skeleton_root->mParent->mNumMeshes == 0)
	{
		aiNode* iterator = skeleton_root->mParent;

		while (iterator->mParent->mNumChildren == 1 && iterator->mParent->mNumMeshes == 0)
			iterator = iterator->mParent;

		for (int i = 0; i < iterator->mParent->mNumChildren; i++)
			if (iterator->mParent->mChildren[i] == iterator)
			{
				iterator->mParent->mChildren[i] = skeleton_root;
				skeleton_root->mParent = iterator->mParent;
				break;
			}
	}
}

aiMatrix4x4 SkeletonImporter::GetGlobalTransform(const aiNode * node) const
{
	if (node->mParent == nullptr)
		return node->mTransformation;

	aiMatrix4x4 transform = node->mTransformation;
	const aiNode* iterator = node->mParent;
	
	while (iterator->mParent != nullptr)
	{
		aiMatrix4x4 inverse(iterator->mParent->mTransformation);
		transform = transform * inverse;
		iterator = iterator->mParent;
	}

	return transform;
}

void SkeletonImporter::ImportJoint(char** iterator, const aiBone * bone, const aiNode* node, aiBone** bones, unsigned int num_bones,const aiMatrix4x4& parent_global_transform) const
{
	memcpy(*iterator, bone->mName.C_Str(), bone->mName.length + 1);
	*iterator += bone->mName.length + 1;

	aiMatrix4x4 offset(bone->mOffsetMatrix);
	offset.Transpose();
	memcpy(*iterator, &offset.a1, sizeof(float) * 4 * 4);
	*iterator += sizeof(float) * 4 * 4;

	aiMatrix4x4 transform = node->mTransformation * parent_global_transform;
	transform.Transpose();
	memcpy(*iterator, &transform.a1, sizeof(float) * 4 * 4);
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

	memcpy(*iterator, &node->mNumChildren, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < node->mNumChildren; i++)
		if (IsBone(node->mChildren[i]->mName, bones, num_bones))
		{
			const aiBone* child_bone = FindEsqueletonBone(node->mChildren[i]->mName, bones, num_bones);
			ImportJoint(iterator, child_bone, node->mChildren[i], bones, num_bones, node->mTransformation * parent_global_transform);
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

	for (int i = 0; i < 4; i++)
	{
		memcpy(&joint.current_inverse_transform[i][0], *iterator, sizeof(float) * 4);
		*iterator += sizeof(float) * 4;
	}

	if (joint.current_inverse_transform.Inverse() == false)
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

const UID SkeletonImporter::Import(aiBone ** bones, aiNode* scene_root_node, aiNode* mesh_node, unsigned int num_bones) const
{
	char format[FORMAT_SIZE] = FORMAT_SKELETON;
	unsigned int buff_size = GetTotalSize(bones, num_bones);
	char* buffer = new char[buff_size];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	aiNode* root_node = FindEsqueletonNode(scene_root_node, bones, num_bones);
	aiBone* root_joint = FindEsqueletonBone(root_node->mName, bones, num_bones);

	aiMatrix4x4 global_mesh(GetGlobalTransform(mesh_node));
	aiMatrix4x4 global_root_joint_parent(GetGlobalTransform(root_node->mParent));
	aiMatrix4x4 root_joint_parent_in_mesh_cords = global_root_joint_parent * global_mesh.Inverse();

	ImportJoint(&iterator, root_joint, root_node, bones, num_bones, root_joint_parent_in_mesh_cords);

	ColapseUselesNodes(root_node);

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

		new_rigg->root_joint = LoadJoint(&iterator);

		to_load->SetRigg(new_rigg);

		return true;
	}

	LOG("Could not read %s", path.c_str());
	return false;
}
