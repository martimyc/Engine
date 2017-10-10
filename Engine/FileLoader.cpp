#include "FileLoader.h"

FileLoader::FileLoader(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

FileLoader::~FileLoader()
{}

bool FileLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool FileLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool FileLoader::LoadScene(const char * path)
{
	bool ret = true;

	char* full_path = "";

	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			ret = LoadMesh(scene->mMeshes[i]);
			if (ret == false)
			{
				LOG("Mesh(%i) did't load correctly", i);
				break;
			}
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", full_path);

	return ret;
}
