#include "UID.h"
#include "Skeleton.h"
#include "FileSystem.h"
#include "Application.h"
#include "SkeletonImporter.h"

SkeletonImporter::SkeletonImporter()
{
}

SkeletonImporter::~SkeletonImporter()
{
}

unsigned int SkeletonImporter::GetTotalSize(aiBone** bones, unsigned int num_bones) const
{
	unsigned int size = FORMAT_SIZE + sizeof(unsigned int);

	for(int i = 0; i < num_bones; i++)
	{ 
		size += bones[i]->mName.length + 1;
		size += sizeof(float) * 3 * 4 + sizeof(unsigned int);
		size += bones[i]->mNumWeights * sizeof(unsigned int) + sizeof(float);
	}

	return size;
}

void SkeletonImporter::ImportJoint(char** iterator, const aiBone * bone) const
{
	memcpy(*iterator, bone->mName.C_Str(), bone->mName.length + 1);
	*iterator += bone->mName.length + 1;

	//TODO check if 0 0 0 1 is the not saved part
	memcpy(*iterator, &bone->mOffsetMatrix.a1, sizeof(float) * 3 * 4);
	*iterator += sizeof(float) * 3 * 4;

	memcpy(*iterator, &bone->mNumWeights, sizeof(unsigned int));
	*iterator += sizeof(unsigned int);

	for (int i = 0; i < bone->mNumWeights; i++)
	{
		memcpy(*iterator, &bone->mWeights->mVertexId, sizeof(unsigned int) + sizeof(float));
		*iterator += sizeof(unsigned int) + sizeof(float);
	}
}

const UID SkeletonImporter::Import(aiBone ** bones, unsigned int num_bones) const
{
	char format[FORMAT_SIZE] = FORMAT_SKELETON;
	unsigned int buff_size = GetTotalSize(bones, num_bones);
	char* buffer = new char[buff_size];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	memcpy(iterator, &num_bones, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	for (int i = 0; i < num_bones; i++)
		ImportJoint( &iterator, bones[i]);

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

		unsigned int num_bones;
		memcpy(&num_bones, iterator, sizeof(unsigned int));
		iterator += sizeof(unsigned int);

		for (uint i = 0; i < num_bones; i++)
		{
			Skeleton::Rigg::Joint joint;
			
			memcpy(&joint.inverse_bind_pose_transform, iterator, sizeof(float) * 3 * 4);
			if (joint.inverse_bind_pose_transform.Inverse())
				LOG("Could not inverse joint transform");
			iterator += sizeof(float) * 3 * 4;

			unsigned int num_weights;
			memcpy(&num_weights, iterator, sizeof(unsigned int));
			iterator += sizeof(unsigned int);

			joint.weights.resize(num_weights);
			memcpy(&joint.weights[0], iterator, (sizeof(unsigned int) + sizeof(float)) * num_weights);
			iterator += (sizeof(unsigned int) + sizeof(float)) * num_weights;
		}

		return true;
	}

	LOG("Could not read %s", path.c_str());
	return false;
}
