#include "Globals.h"
#include "UID.h"
#include "Animation.h"
#include "AnimationAsset.h"
#include "AssetDirectory.h"
#include "FileSystem.h"
#include "Application.h"
#include "AnimationImporter.h"

AnimationImporter::AnimationImporter()
{}

AnimationImporter::~AnimationImporter()
{}

unsigned int AnimationImporter::GetTotalSize(const aiAnimation * animation) const
{
	unsigned int size = FORMAT_SIZE;
	size += sizeof(unsigned int);
	size += sizeof(double);
	size += sizeof(double);

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		size += animation->mChannels[i]->mNodeName.length + 1;
		size += sizeof(unsigned int) * 3;
		size += animation->mChannels[i]->mNumPositionKeys * (sizeof(double) + sizeof(float) * 3);
		size += animation->mChannels[i]->mNumRotationKeys * (sizeof(double) + sizeof(float) * 4);
		size += animation->mChannels[i]->mNumScalingKeys * (sizeof(double) + sizeof(float) * 3);
	}

	return size;
}

const UID AnimationImporter::Import(const aiAnimation * animation, const AnimationImportConfiguration * config) const
{
	char* buffer = new char[GetTotalSize(animation)];
	char* iterator = buffer;

	char format[FORMAT_SIZE] = FORMAT_ANIMATION;
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	memcpy(iterator, &animation->mNumChannels, sizeof(unsigned int));
	iterator += sizeof(unsigned int);

	memcpy(iterator, &animation->mTicksPerSecond, sizeof(double));
	iterator += sizeof(double);

	memcpy(iterator, &animation->mDuration, sizeof(double));
	iterator += sizeof(double);

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		memcpy(iterator, animation->mChannels[i]->mNodeName.C_Str(), animation->mChannels[i]->mNodeName.length + 1);
		iterator += animation->mChannels[i]->mNodeName.length + 1;

		memcpy(iterator, &animation->mChannels[i]->mNumPositionKeys, sizeof(unsigned int));
		iterator += sizeof(unsigned int);

		memcpy(iterator, &animation->mChannels[i]->mNumRotationKeys, sizeof(unsigned int));
		iterator += sizeof(unsigned int);

		memcpy(iterator, &animation->mChannels[i]->mNumScalingKeys, sizeof(unsigned int));
		iterator += sizeof(unsigned int);

		//Positions
		for (int j = 0; j < animation->mChannels[i]->mNumPositionKeys; j++)
		{
			memcpy(iterator, &animation->mChannels[i]->mPositionKeys[j].mTime, sizeof(double));
			iterator += sizeof(double);

			memcpy(iterator, &animation->mChannels[i]->mPositionKeys[j].mValue.x, sizeof(float) * 3);
			iterator += sizeof(float) * 3;
		}

		//Rotations
		for (int j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
		{
			memcpy(iterator, &animation->mChannels[i]->mRotationKeys[j].mTime, sizeof(double));
			iterator += sizeof(double);

			memcpy(iterator, &animation->mChannels[i]->mRotationKeys[j].mValue.x, sizeof(float) * 3);
			iterator += sizeof(float) * 3;

			memcpy(iterator, &animation->mChannels[i]->mRotationKeys[j].mValue.w, sizeof(float));
			iterator += sizeof(float);
		}

		//Scaling
		for (int j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
		{
			memcpy(iterator, &animation->mChannels[i]->mScalingKeys[j].mTime, sizeof(double));
			iterator += sizeof(double);

			memcpy(iterator, &animation->mChannels[i]->mScalingKeys[j].mValue.x, sizeof(float) * 3);
			iterator += sizeof(float) * 3;
		}
	}

	uint length = iterator - buffer;

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, App->file_system->GetAnimations().c_str(), id.GetAsName(), "mm") == false)
	{
		LOG("Could not save animation correctlly");
		return UID();
	}

	delete[] buffer;

	return id;
}

bool AnimationImporter::Load(Animation * to_load, const AnimationLoadConfiguration * config) const
{
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetAnimations());
	path += "\\";
	path += to_load->GetUID().GetAsName();
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		Animation::AnimationClip* new_clip = new Animation::AnimationClip;

		unsigned int num_channels;
		memcpy(&num_channels, iterator, sizeof(unsigned int));
		iterator += sizeof(unsigned int);

		memcpy(&new_clip->ticks_per_sec, iterator, sizeof(double));
		iterator += sizeof(double);

		memcpy(&new_clip->duration, iterator, sizeof(double));
		iterator += sizeof(double);

		new_clip->channels.reserve(num_channels);
		Animation::AnimationClip::Channel* new_channel;

		for (unsigned int i = 0; i < num_channels; i++)
		{
			new_channel = new Animation::AnimationClip::Channel;

			new_channel->joint_name = iterator;
			iterator += new_channel->joint_name.length() + 1;

			unsigned int num_keys[3];
			memcpy(num_keys, iterator, sizeof(unsigned int) * 3);
			iterator += sizeof(unsigned int) * 3;

			//Positions
			new_channel->position_samples.resize(num_keys[0]);
			for (int i = 0; i < num_keys[0]; i++)
			{
				memcpy(&new_channel->position_samples[i], iterator, sizeof(double) + sizeof(float) * 3);
				iterator += sizeof(double) + sizeof(float) * 3;
			}
			std::sort(new_channel->position_samples.begin(), new_channel->position_samples.end(), CompareVec());

			//Rotations
			new_channel->rotation_samples.resize(num_keys[1]);
			for (int i = 0; i < num_keys[1]; i++)
			{
				memcpy(&new_channel->rotation_samples[i], iterator, sizeof(double) + sizeof(float) * 4);
				iterator += sizeof(double) + sizeof(float) * 4;
			}
			std::sort(new_channel->rotation_samples.begin(), new_channel->rotation_samples.end(), CompareQuat());

			//Scaling
			new_channel->scale_samples.resize(num_keys[2]);
			for (int i = 0; i < num_keys[2]; i++)
			{
				memcpy(&new_channel->scale_samples[i], iterator, sizeof(double) + sizeof(float) * 3);
				iterator += sizeof(double) + sizeof(float) * 3;
			}
			std::sort(new_channel->scale_samples.begin(), new_channel->scale_samples.end(), CompareVec());

			new_clip->channels.push_back(new_channel);
		}

		to_load->SetAnimationClip(new_clip);

		delete[] buffer;
		return true;
	}
	LOG("Could not read %s", path.c_str());
	return false;
}
