#ifndef _MODULE_FILE_LOADER
#define _MODULE_FILE_LOADER


#include "Assimp\include\cimport.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "Module.h"

class Mesh;

class FileLoader : public Module
{
private:

public:
	FileLoader(const char* name, bool start_enabled = true);
	~FileLoader();

	bool Init();
	bool CleanUp();

	//Loads
	bool LoadScene(const char* path);
	const Mesh* LoadMesh(const aiMesh* mesh);
};

#endif _MODULE_FILE_LOADER