#include "MeshLoader.h"

MeshLoader::MeshLoader(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

MeshLoader::~MeshLoader()
{}

bool MeshLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	return true;
}
