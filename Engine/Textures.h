#ifndef _MODULE_TEXTURES
#define _MODULE_TEXTURES

#include <list>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Module.h"

class Textures: public Module
{
private:
	std::list <GLuint> textures;
public:
	Textures(const char* name, bool start_enabled = true);

	bool Init();

	GLuint LoadTexture(const char* path, bool hiest_quality = true);
};

#endif // !_MODULE_TEXTURES
