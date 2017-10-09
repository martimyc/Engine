#ifndef _MODULE_MESH_LOADER
#define _MODULE_MESH_LOADER

#include "assimp\include\cimport.h"
#include "assimp\include\cimport.h"
#include "assimp\include\scene.h"
#include "assimp\include\postprocess.h"
#include "assimp\include\cfileio.h"
#include "Module.h"

class MeshLoader : public Module
{
private:

public:
	MeshLoader(const char* name, bool start_enabled = true);
	~MeshLoader();

	bool Init();
};

#endif