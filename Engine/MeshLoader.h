#ifndef _MODULE_MESH_LOADER
#define _MODULE_MESH_LOADER


#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
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