#ifndef _MODULE_TEXTURES
#define _MODULE_TEXTURES

#include <list>
#include "glew\include\GL\glew.h"
#include "DevIL\include\IL\il.h"
#include "DevIL\include\IL\ilu.h"
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
